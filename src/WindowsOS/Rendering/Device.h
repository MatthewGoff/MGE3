#ifndef DEVICE_H
#define DEVICE_H

#include "Struct.h"

namespace WindowsOS {
namespace Rendering
{
    
class Device
{
    /*
    Abstraction of the physical graphics card.
    */
    
    public:
    VkDevice LogicalDevice;
    
    bool CreateDevice(
        VulkanConfig* config,
        VkPhysicalDevice physical_device,
        QueueFamilyConfig* queue_family_config,    
        VkQueue* graphics_queue,
        VkQueue* present_queue);

    bool AllocateDeviceMemory(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkDeviceMemory* allocation);

    bool BindBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer* buffer,
        VkDeviceMemory* memory);

    bool CreateBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer* buffer,
        VkDeviceMemory* buffer_memory);

    bool FindMemoryType(
        uint32 typeFilter,
        VkMemoryPropertyFlags properties,
        uint32 &out);

    bool CreateImageAllocation(
        uint64 size,
        VkDeviceMemory* image_memory);

    bool BindImage(
        uint32 width,
        uint32 height,
        VkImage* image,
        VkDeviceMemory* image_memory);

    bool CreateImage(
        uint32 width,
        uint32 height,
        uint64 size,
        VkImage* image,
        VkDeviceMemory* image_memory);

    VkPhysicalDeviceProperties PhysicalProperties;

    private:
    VkPhysicalDevice PhysicalDevice;
    
    VkPhysicalDeviceMemoryProperties MemoryProperties;
};

}}

#endif