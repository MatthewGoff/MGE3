#include "..\Struct.h"

namespace WindowsOS { namespace Rendering {
namespace Init
{
    QueueFamilies FindQueueFamilies(
        VkPhysicalDevice physical_device_handle, 
        VkSurfaceKHR surface_handle)
    {
        QueueFamilies queue_families = {};
        
        uint32 count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(
            physical_device_handle,
            &count,
            nullptr);
        
        if (count == 0)
        {
            return queue_families;
        }
        if (count > 10)
        {
            Error("[Error] More than len(buffer) queue families found.\n");
            return queue_families;
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
                queue_families.GraphicsAvailable = true;
                queue_families.GraphicsIndex = i;
            }
            VkBool32 presentSupport;
            vkGetPhysicalDeviceSurfaceSupportKHR(
                physical_device_handle,
                i,
                surface_handle,
                &presentSupport);
            if (presentSupport)
            {
                queue_families.PresentAvailable = true;
                queue_families.PresentIndex = i;
            }
        }
        
        return queue_families;
    }
    
    /*
    Return false if failed to complete or if swapchain support is inssuficient.
    */
    bool DetermineSwapchainSupport(
        VkPhysicalDevice physical_device_handle,
        VkSurfaceKHR surface_handle,
        SwapchainSupport* swapchain_support)
    {
        *swapchain_support = {};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            physical_device_handle,
            surface_handle,
            &swapchain_support->Capabilities);

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
        
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            physical_device_handle,
            surface_handle,
            &count,
            swapchain_support->Formats);
        swapchain_support->FormatsCount = count;

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

        vkGetPhysicalDeviceSurfacePresentModesKHR(
            physical_device_handle,
            surface_handle,
            &count,
            swapchain_support->PresentModes);
        swapchain_support->PresentModesCount = count;

        /*
        Check for swapchain being addequate: (or another function)
        swapChainAdequate = !swapChainSupport.formats.empty() &&
        !swapChainSupport.presentModes.empty();
        */
        
        return true;
    }
    
    bool DeviceSupportsExtensions(
        VkConfig* config,
        VkPhysicalDevice physical_device_handle)
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
        VkConfig* config,
        VkPhysicalDevice physical_device_handle,
        VkSurfaceKHR surface_handle,
        QueueFamilies* queue_families,
        SwapchainSupport* swapchain_support)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physical_device_handle, &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physical_device_handle, &features);
        
        *queue_families = FindQueueFamilies(
            physical_device_handle,
            surface_handle);
        if (
            !queue_families->GraphicsAvailable ||
            !queue_families->PresentAvailable)
        {
            return false;
        }
        
        bool extension_support = DeviceSupportsExtensions(
            config,
            physical_device_handle);
        if (!extension_support)
        {
            return false;
        }

        bool swap_chain_support = DetermineSwapchainSupport(
            physical_device_handle,
            surface_handle,
            swapchain_support);
        if (!swap_chain_support)
        {
            return false;
        }

        return true;
    }
    
    bool SelectPhysicalDevice(
        VkConfig* config,
        VkInstance vulkan_handle,
        VkSurfaceKHR surface_handle,
        QueueFamilies* queue_families,
        SwapchainSupport* swapchain_support,
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
                queue_families,
                swapchain_support);
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