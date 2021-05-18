#include "..\Struct.h"

namespace WindowsOS { namespace Rendering {
namespace Init
{
    QueueFamilySupport FindQueueFamilySupport(
        VkPhysicalDevice physical_device_handle, 
        VkSurfaceKHR surface_handle)
    {
        QueueFamilySupport queue_family_support = {};
        
        uint32 count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(
            physical_device_handle,
            &count,
            nullptr);
        
        if (count == 0)
        {
            return queue_family_support;
        }
        if (count > 10)
        {
            Error("[Error] More than len(buffer) queue families found.\n");
            return queue_family_support;
        }
        VkQueueFamilyProperties family_properties[10];
        vkGetPhysicalDeviceQueueFamilyProperties(
            physical_device_handle,
            &count,
            family_properties);
        
        uint32 result;
        for (int i = 0; i < count; i++)
        {
            if (family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                queue_family_support.GraphicsAvailable = true;
                queue_family_support.GraphicsIndex = i;
            }
            VkBool32 present_support;
            vkGetPhysicalDeviceSurfaceSupportKHR(
                physical_device_handle,
                i,
                surface_handle,
                &present_support);
            if (present_support)
            {
                queue_family_support.PresentAvailable = true;
                queue_family_support.PresentIndex = i;
            }
        }
        
        return queue_family_support;
    }
    
    VkPresentModeKHR ChooseSwapPresentMode(VkPresentModeKHR* available_modes, int count)
    {
        for (int i = 0; i < count; i++)
        {
            VkPresentModeKHR mode = *(available_modes + i);
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return mode;
            }
        }
        
        return VK_PRESENT_MODE_FIFO_KHR;
    }
    
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(VkSurfaceFormatKHR* available_formats, int count)
    {
        Assert(count!=0);
        for (int i = 0; i < count; i++) {
            VkSurfaceFormatKHR* format = (available_formats + i);
            if (format->format == VK_FORMAT_B8G8R8A8_SRGB &&
                format->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return *format;
            }
        }
        
        // First element of array is returned by default
        return *available_formats;
    }
    
    /*
    Return false if failed to complete or if swapchain support is inssuficient.
    */
    bool DetermineSwapchainSupport(
        VkPhysicalDevice physical_device_handle, // in
        VkSurfaceKHR surface_handle, // in
        SwapchainConfig* swapchain_config) // out
    {
        *swapchain_config = {};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            physical_device_handle,
            surface_handle,
            &swapchain_config->Capabilities);

        uint32 count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            physical_device_handle,
            surface_handle,
            &count,
            nullptr);
        
        if (count == 0 || count > 20)
        {
            return false;
        }
        
        VkSurfaceFormatKHR available_formats[20];
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            physical_device_handle,
            surface_handle,
            &count,
            available_formats);
        
        swapchain_config->SurfaceFormat = ChooseSwapSurfaceFormat(available_formats, count);

        count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            physical_device_handle,
            surface_handle,
            &count,
            nullptr);
        
        if (count == 0 || count > 20)
        {
            return false;
        }

        VkPresentModeKHR available_modes[20];
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            physical_device_handle,
            surface_handle,
            &count,
            available_modes);
                    
        swapchain_config->PresentMode = ChooseSwapPresentMode(available_modes, count);
        
        return true;
    }
    
    bool DeviceSupportsExtensions(VulkanConfig* config, VkPhysicalDevice physical_device_handle)
    {
        uint32 count;
        vkEnumerateDeviceExtensionProperties(
            physical_device_handle,
            nullptr,
            &count,
            nullptr);

        if (count == 0)
        {
            return false;
        }
        if (count > 200)
        {
            Warning("[Warning] More than len(buffer) extension found.\n");
            return false;
        }
        
        VkExtensionProperties available_extensions[200];
        vkEnumerateDeviceExtensionProperties(
            physical_device_handle,
            nullptr,
            &count,
            available_extensions);

        for (int i = 0; i < config->DeviceExtensionsCount; i++)
        {
            char* desired_extension = (char*) config->DeviceExtensions[i];
            bool extension_found = false;

            for (int j = 0; j < count; j++)
            {
                VkExtensionProperties extension = available_extensions[j];
                int comp = String::Compare(
                    desired_extension,
                    extension.extensionName);
                if (comp == 0)
                {
                    extension_found = true;
                    break;
                }
            }

            if (!extension_found) {
                return false;
            }
        }

        return true;
    }

    bool SuitableDevice(
        VulkanConfig* config,
        VkPhysicalDevice physical_device_handle,
        VkSurfaceKHR surface_handle,
        QueueFamilySupport* queue_family_support,
        SwapchainConfig* swapchain_config)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physical_device_handle, &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physical_device_handle, &features);
        
        *queue_family_support = FindQueueFamilySupport(physical_device_handle, surface_handle);
        if (!queue_family_support->GraphicsAvailable || !queue_family_support->PresentAvailable)
        {
            return false;
        }
        
        bool extension_support = DeviceSupportsExtensions(config, physical_device_handle);
        if (!extension_support)
        {
            return false;
        }

        bool swap_chain_support = DetermineSwapchainSupport(
            physical_device_handle,
            surface_handle,
            swapchain_config);
        if (!swap_chain_support)
        {
            return false;
        }

        return true;
    }
    
    bool SelectPhysicalDevice(
        VulkanConfig* config,
        VkInstance vulkan_handle,
        VkSurfaceKHR surface_handle,
        QueueFamilySupport* queue_family_support,
        SwapchainConfig* swapchain_config, //out
        VkPhysicalDevice* physical_device_handle)
    {
        uint32 count = 0;
        vkEnumeratePhysicalDevices(vulkan_handle, &count, nullptr);
        
        if (count == 0)
        {
            return false;
        }
        if (count > 10)
        {
            return false;
        }
        
        VkPhysicalDevice available_devices[10];
        vkEnumeratePhysicalDevices(vulkan_handle, &count, available_devices);
        
        for (int i = 0; i < count; i++)
        {
            if (available_devices[i] == nullptr) continue;
            bool suitable = SuitableDevice(
                config,
                available_devices[i],
                surface_handle,
                queue_family_support,
                swapchain_config);
            if (suitable)
            {
                *physical_device_handle = available_devices[i];
                return true;
            }
        }
        
        return false;
    }
}
} }