#include "..\Struct.h"

namespace WindowsOS { namespace Rendering {
namespace Init
{    
    bool CreateImageViews(
        VulkanEnvironment* env,
        SwapchainConfig* swapchain_config)
    {
        for (int i = 0; i < swapchain_config->Size; i++)
        {
            VkImageViewCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            create_info.image = env->Images[i];
            create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            create_info.format = swapchain_config->SurfaceFormat.format;
            
            create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            create_info.subresourceRange.baseMipLevel = 0;
            create_info.subresourceRange.levelCount = 1;
            create_info.subresourceRange.baseArrayLayer = 0;
            create_info.subresourceRange.layerCount = 1;
            
            VkResult result = vkCreateImageView(
                env->LogicalDevice,
                &create_info,
                nullptr,
                &env->ImageViews[i]);
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
        VulkanEnvironment* env,
        QueueFamilyConfig* queue_family_config,
        SwapchainConfig* swapchain_config)
    {
        swapchain_config->Extent = ChooseSwapExtent(&swapchain_config->Capabilities);
        
        uint32 image_count = swapchain_config->Capabilities.minImageCount + 1;
        
        // 0 is a reserved value indicating there is no maximum
        if (swapchain_config->Capabilities.maxImageCount != 0)
        {
            image_count = Util::Min(image_count, swapchain_config->Capabilities.maxImageCount);
        }
        
        if (image_count > SwapchainConfig::MAX_SIZE)
        {
            Error("[Error] Swapchain images > len(buffer)\n");
            return false;
        }
        
        swapchain_config->Size = image_count;
                
        VkSwapchainCreateInfoKHR create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface = env->Surface;
        create_info.minImageCount = swapchain_config->Size;
        create_info.imageFormat = swapchain_config->SurfaceFormat.format;
        create_info.imageColorSpace = swapchain_config->SurfaceFormat.colorSpace;
        create_info.imageExtent = swapchain_config->Extent;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        
        uint32 indices[] = {
            (uint32)queue_family_config->GraphicsIndex,
            (uint32)queue_family_config->PresentIndex};
        
        if (queue_family_config->GraphicsIndex != queue_family_config->PresentIndex)
        {
            create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices = indices;
        }
        else
        {
            create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            create_info.queueFamilyIndexCount = 0; // Optional
            create_info.pQueueFamilyIndices = nullptr; // Optional
        }

        create_info.preTransform = swapchain_config->Capabilities.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode = swapchain_config->PresentMode;
        create_info.clipped = VK_TRUE;
        create_info.oldSwapchain = VK_NULL_HANDLE;
        
        VkResult result = vkCreateSwapchainKHR(
            env->LogicalDevice,
            &create_info,
            nullptr,
            &env->Swapchain);
        if (result != VK_SUCCESS)
        {
            return false;
        }
        
        int temp = swapchain_config->Size;
        
        // This call should not be neccesary but there is a validation error without it.
        vkGetSwapchainImagesKHR(
            env->LogicalDevice,
            env->Swapchain,
            (uint32*)&swapchain_config->Size,
            nullptr);

        Assert(temp == swapchain_config->Size);

        vkGetSwapchainImagesKHR(
            env->LogicalDevice,
            env->Swapchain,
            (uint32*)&swapchain_config->Size,
            (VkImage*)env->Images);

        bool success = CreateImageViews(env, swapchain_config);
        if (!success)
        {
            return false;
        }
        
        return true;
    }

}
} }