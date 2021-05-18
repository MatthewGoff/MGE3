#include "..\Struct.h"

namespace WindowsOS { namespace Rendering {
namespace Init
{    
    bool CreateImageViews(
        VkDevice logical_device_handle,
        int count,
        SwapchainConfig* swapchain_config,
        VkImage* images,
        VkImageView* image_views)
    {
        for (int i = 0; i < count; i++)
        {
            VkImageViewCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = *(images + i);
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapchain_config->SurfaceFormat.format;
            
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;
            
            VkResult result = vkCreateImageView(
                logical_device_handle,
                &createInfo,
                nullptr,
                image_views + i);
            if (result != VK_SUCCESS)
            {            
                Error("[Error] Vulkan failed to create image view\n");
                return false;
            }
        }
        
        return true;
    }

    VkExtent2D ChooseSwapExtent(VkSurfaceCapabilitiesKHR* capabilities)
    {
        if (capabilities->currentExtent.width != UINT32_MAX)
        {
            return capabilities->currentExtent;
        }
        else
        {
            VkExtent2D extent = {WINDOW_WIDTH, WINDOW_HEIGHT};
            
            extent.width = Util::Min(extent.width, capabilities->maxImageExtent.width);
            extent.width = Util::Max(extent.width, capabilities->minImageExtent.width);
            
            extent.height = Util::Min(extent.height, capabilities->maxImageExtent.height);
            extent.height = Util::Max(extent.height, capabilities->minImageExtent.height);
            
            return extent;
        }
    }

    bool CreateSwapchain(
        VkDevice logical_device_handle,
        VkSurfaceKHR surface_handle,
        QueueFamilySupport* queue_family_support,
        SwapchainConfig* swapchain_config,
        VkImageView* image_views,
        VkSwapchainKHR* swapchain_handle)
    {
        VkSurfaceFormatKHR surface_format = swapchain_config->SurfaceFormat;
        VkPresentModeKHR present_mode = swapchain_config->PresentMode;

        VkExtent2D extent = ChooseSwapExtent(&swapchain_config->Capabilities);
        
        uint32 image_count = swapchain_config->Capabilities.minImageCount + 1;
        
        // 0 is a reserved value indicating there is no maximum
        if (swapchain_config->Capabilities.maxImageCount != 0)
        {
            image_count = Util::Min(image_count, swapchain_config->Capabilities.maxImageCount);
        }
        
        if (image_count > 20)
        {
            Error("[Error] Swapchain images > len(buffer)\n");
            return false;
        }
        
        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface_handle;
        createInfo.minImageCount = image_count;
        createInfo.imageFormat = surface_format.format;
        createInfo.imageColorSpace = surface_format.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        
        uint32 indices[] = {
            (uint32)queue_family_support->GraphicsIndex,
            (uint32)queue_family_support->PresentIndex};
        
        if (queue_family_support->GraphicsIndex != queue_family_support->PresentIndex)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = indices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo.preTransform = swapchain_config->Capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = present_mode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;
        
        VkResult result = vkCreateSwapchainKHR(
            logical_device_handle,
            &createInfo,
            nullptr,
            swapchain_handle);
        if (result != VK_SUCCESS)
        {
            return false;
        }
        
        
        VkImage swapchain_images[20];
        { // Initialize swap chain images
            // First call to get the count. Theoretically the count is the same as it was when we sent it as a parameter.
            vkGetSwapchainImagesKHR(
                logical_device_handle,
                *swapchain_handle,
                &image_count,
                nullptr);
            vkGetSwapchainImagesKHR(
                logical_device_handle,
                *swapchain_handle,
                &image_count,
                swapchain_images);
        }
        
        swapchain_config->SurfaceFormat = surface_format;
        swapchain_config->Extent = extent;
        swapchain_config->Size = image_count;

        bool success = CreateImageViews(
            logical_device_handle,
            image_count,
            swapchain_config,
            swapchain_images,
            image_views);
        if (!success)
        {
            return false;
        }
        
        return true;
    }

}
} }