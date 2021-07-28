#pragma once

#include "Struct.h"

namespace MGE { namespace Windows { namespace Rendering {

/*
Abstraction of the physical graphics card.
*/
class Device
{

    public:
    VkDevice LogicalDevice;

    Buffer UniformBuffer;
    Buffer VertexBuffer;
    Buffer StagingBuffer;

    VkPhysicalDeviceProperties PhysicalProperties;

    bool Init(
        VulkanConfig* config,
        VkPhysicalDevice physical_device,
        QueueFamilyConfig* queue_family_config,    
        VkQueue* graphics_queue,
        VkQueue* present_queue);

    bool CreateImage(uint32 width, uint32 height, VkImage* image, uint64* memory_offset);

    private:
    
    DeviceAllocation TextureMemory;

    VkPhysicalDevice PhysicalDevice;
    VkPhysicalDeviceMemoryProperties MemoryProperties;
    
    bool InitializeBuffers();
    
    bool AllocateDeviceMemory(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkDeviceMemory* allocation);

    bool CreateBuffer(Buffer* buffer);

    bool CreateImageAllocation(uint64 size, VkDeviceMemory* image_memory);

    bool FindMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties, uint32* out);
};

}}}