#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan.h>

#include "Device.h"

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
bool Device::FindMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties, uint32 &out)
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

bool Device::AllocateDeviceMemory(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkDeviceMemory* allocation)
{
    VkBuffer model_buffer;
    
    VkBufferCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    create_info.size = size;
    create_info.usage = usage;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    VkResult result = vkCreateBuffer(LogicalDevice, &create_info, nullptr, &model_buffer);
    if (result != VK_SUCCESS) {return false;}
    
    VkMemoryRequirements mem_req;
    vkGetBufferMemoryRequirements(LogicalDevice, model_buffer, &mem_req);
    
    uint32 memory_type;
    bool success = FindMemoryType(mem_req.memoryTypeBits, properties, memory_type);
    if (!success) {return false;}
    
    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_req.size;
    alloc_info.memoryTypeIndex = memory_type;
    
    Info("[Info] Buffer size = ");
    Info(alloc_info.allocationSize);
    Info(".\n");
    
    // for reference: vkFreeMemory(logical_device, buffer_memory, nullptr)
    result = vkAllocateMemory(LogicalDevice, &alloc_info, nullptr, allocation);
    if (result != VK_SUCCESS) {return false;}
    
    vkDestroyBuffer(LogicalDevice, model_buffer, nullptr);
    return true;
}

bool Device::BindBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer* buffer,
    VkDeviceMemory* memory)
{
    VkBufferCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    create_info.size = size;
    create_info.usage = usage;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    VkResult result = vkCreateBuffer(LogicalDevice, &create_info, nullptr, buffer);
    if (result != VK_SUCCESS) {return false;}
    
    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(LogicalDevice, *buffer, &mem_requirements);
    
    result = vkBindBufferMemory(LogicalDevice, *buffer, *memory, 0);
    if (result != VK_SUCCESS) {return false;}
    
    return true;
}

bool Device::CreateBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer* buffer,
    VkDeviceMemory* buffer_memory)
{
    AllocateDeviceMemory(size, usage, properties, buffer_memory);
        
    BindBuffer(size, usage, properties, buffer, buffer_memory);
    
    return true;
}

bool Device::CreateImageAllocation(uint64 size, VkDeviceMemory* allocation)
{
    VkImage model_image;
    
    VkImageCreateInfo image_info = {};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = 32; // arbitrary
    image_info.extent.height = 32; // arbitrary
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = VK_FORMAT_R8G8B8A8_SRGB;
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.flags = 0; // Optional

    VkResult result = vkCreateImage(LogicalDevice, &image_info, nullptr, &model_image);
    if (result != VK_SUCCESS) {return false;}
    
    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(LogicalDevice, model_image, &memory_requirements);
    
    bool success;
    uint32 memory_type;
    success = FindMemoryType(
        memory_requirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        memory_type);
    if (!success)
    {
        return false;
    }

    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = size;
    alloc_info.memoryTypeIndex = memory_type;
    
    Info("[Info] Image size = ");
    Info(alloc_info.allocationSize);
    Info(".\n");
    
    result = vkAllocateMemory(LogicalDevice, &alloc_info, nullptr, allocation);
    if (result != VK_SUCCESS) {return false;}
    
    return true;
}

bool Device::BindImage(uint32 width, uint32 height, VkImage* image, VkDeviceMemory* image_memory)
{
    VkImageCreateInfo image_info = {};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = width;
    image_info.extent.height = height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = VK_FORMAT_R8G8B8A8_SRGB;
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.flags = 0; // Optional

    VkResult result = vkCreateImage(LogicalDevice, &image_info, nullptr, image);
    if (result != VK_SUCCESS) {return false;}
    
    result = vkBindImageMemory(LogicalDevice, *image, *image_memory, 0);
    if (result != VK_SUCCESS) {return false;}
    
    return true;
}

bool Device::CreateImage(
    uint32 width,
    uint32 height,
    uint64 size,
    VkImage* image,
    VkDeviceMemory* image_memory)
{
    bool success;
    
    success = CreateImageAllocation(size, image_memory);
    if (!success) {return false;}
    
    success = BindImage(width, height, image, image_memory);
    if (!success) {return false;}
    
    return true;
}

}
}