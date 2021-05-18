#include "..\Struct.h"

namespace WindowsOS { namespace Rendering {
namespace Init
{
    bool CreateLogicalDevice(
        VulkanConfig* config,
        VkPhysicalDevice physical_device_handle,
        QueueFamilyConfig* queue_family_config,    
        VkQueue* GraphicsQueue,
        VkQueue* PresentQueue,
        VkDevice* logical_device_handle)
    {
        VkDeviceQueueCreateInfo queueCreateInfo[1];
        float queuePriority = 1.0f;
        // One entry for each unique queue family index
        if (queue_family_config->GraphicsIndex == queue_family_config->PresentIndex)
        {
            queueCreateInfo[0] = {};
            queueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo[0].queueFamilyIndex = queue_family_config->GraphicsIndex;
            queueCreateInfo[0].queueCount = 1;
            float queuePriority = 1.0f;
            queueCreateInfo[0].pQueuePriorities = &queuePriority;
        }
        
        VkPhysicalDeviceFeatures deviceFeatures = {};
        
        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledLayerCount = config->ValidationLayersCount;
        createInfo.ppEnabledLayerNames = (char**)config->ValidationLayers;
        createInfo.queueCreateInfoCount = sizeof(queueCreateInfo)/sizeof(queueCreateInfo[0]);
        createInfo.pQueueCreateInfos = queueCreateInfo;
        createInfo.enabledExtensionCount = config->DeviceExtensionsCount;
        createInfo.ppEnabledExtensionNames = (char**)config->DeviceExtensions;

        VkResult result = vkCreateDevice(
            physical_device_handle,
            &createInfo,
            nullptr,
            logical_device_handle);
        if (result != VK_SUCCESS)
        {
            Error("[Error] Failed to create logical device with result: ");
            Error(result);
            Error("\n");
            return false;
        }
        
        // Spec says they are void return
        vkGetDeviceQueue(
            *logical_device_handle,
            queue_family_config->GraphicsIndex,
            0,
            GraphicsQueue);
        vkGetDeviceQueue(
            *logical_device_handle,
            queue_family_config->PresentIndex,
            0,
            PresentQueue);
        
        return true;
    }
}
} }