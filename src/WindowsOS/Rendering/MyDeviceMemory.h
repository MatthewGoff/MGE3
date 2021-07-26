#ifndef MY_DEVICE_MEMORY_H
#define MY_DEVICE_MEMORY_H

#include <vulkan.h>

namespace WindowsOS {
namespace Rendering
{
    struct MyDeviceMemory {
        uint64 UniformBufferSize = 1 * KILOBYTES;
        VkBuffer UniformBuffer;
        VkDeviceMemory UniformBufferMemory;
        
        uint64 VertexBufferSize = 1 * KILOBYTES;
        VkBuffer VertexBuffer;
        VkDeviceMemory VertexBufferMemory;
        
        uint64 TextureSize = 64 * KILOBYTES;
        VkImage Texture;
        VkDeviceMemory TextureMemory;
        
        uint64 StagingBufferSize = 64 * KILOBYTES;
        VkBuffer StagingBuffer;
        VkDeviceMemory StagingBufferMemory;
    };
}}

#endif