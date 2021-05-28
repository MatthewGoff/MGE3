#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan.h>

#include "Struct.h"

namespace WindowsOS {
namespace Rendering {

bool Device::CreateDevice(
    VulkanConfig* config,
    VkPhysicalDevice physical_device,
    QueueFamilyConfig* queue_family_config,    
    VkQueue* graphics_queue,
    VkQueue* present_queue)
{
    PhysicalDevice = physical_device;
    vkGetPhysicalDeviceProperties(PhysicalDevice, &PhysicalProperties);

    VkDeviceQueueCreateInfo queue_info[1];
    float queue_priority = 1.0f;
    // One entry for each unique queue family index
    if (queue_family_config->GraphicsIndex == queue_family_config->PresentIndex)
    {
        queue_info[0] = {};
        queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info[0].queueFamilyIndex = queue_family_config->GraphicsIndex;
        queue_info[0].queueCount = 1;
        float queue_priority = 1.0f;
        queue_info[0].pQueuePriorities = &queue_priority;
    }
    
    VkPhysicalDeviceFeatures device_features = {};
    device_features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo device_info = {};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.pEnabledFeatures = &device_features;
    device_info.enabledLayerCount = config->ValidationLayersCount;
    device_info.ppEnabledLayerNames = (char**)config->ValidationLayers;
    device_info.queueCreateInfoCount = sizeof(queue_info)/sizeof(queue_info[0]);
    device_info.pQueueCreateInfos = queue_info;
    device_info.enabledExtensionCount = config->DeviceExtensionsCount;
    device_info.ppEnabledExtensionNames = (char**)config->DeviceExtensions;

    VkResult result = vkCreateDevice(
        PhysicalDevice,
        &device_info,
        nullptr,
        &LogicalDevice);
    if (result != VK_SUCCESS)
    {
        Error("[Error] Failed to create logical device with result: ");
        Error(result);
        Error("\n");
        return false;
    }
    
    // Spec says they are void return
    vkGetDeviceQueue(
        LogicalDevice,
        queue_family_config->GraphicsIndex,
        0,
        graphics_queue);
    vkGetDeviceQueue(
        LogicalDevice,
        queue_family_config->PresentIndex,
        0,
        present_queue);
    
    return true;
}

/*
FindMemoryType:
Find a type of memory (available on physical_device) which has the desired properties provided
by param "properties"
*/
bool Device::FindMemoryType(
    uint32 typeFilter,
    VkMemoryPropertyFlags properties,
    uint32 &out)
{
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &mem_properties);

    for (uint32 i = 0; i < mem_properties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i))
            && ((mem_properties.memoryTypes[i].propertyFlags & properties) == properties))
        {
            out = i;
            return true;
        }
    }
    
    return false;
    //for reference: "failed to find suitable memory type!"
}

bool Device::CreateBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer* buffer,
    VkDeviceMemory* buffer_memory)
{
    VkBufferCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    create_info.size = size;
    create_info.usage = usage;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    VkResult result = vkCreateBuffer(
        LogicalDevice,
        &create_info,
        nullptr,
        buffer);
    if (result != VK_SUCCESS)
    {
        return false;
    }
    
    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(
        LogicalDevice,
        *buffer,
        &mem_requirements);
    
    uint32 memory_type;
    bool success = FindMemoryType(
        mem_requirements.memoryTypeBits,
        properties,
        memory_type);
    if (!success) {return false;}
    
    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = memory_type;
    
    // for reference: vkFreeMemory(logical_device, buffer_memory, nullptr)
    result = vkAllocateMemory(
        LogicalDevice,
        &alloc_info,
        nullptr,
        buffer_memory);
    if (result != VK_SUCCESS)
    {
        return false;
    }
    
    result = vkBindBufferMemory(LogicalDevice, *buffer, *buffer_memory, 0);
    if (result != VK_SUCCESS)
    {
        return false;
    }
    
    return true;
}

}
}