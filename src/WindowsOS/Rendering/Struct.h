#ifndef STRUCT_H
#define STRUCT_H

#include <vulkan.h>

namespace WindowsOS {
namespace Rendering
{
    
    struct UniformBufferObject
    {
        Vector::float2 CameraPosition;
        Vector::float2 CameraDimensions;
    };

    struct Vertex
    {
        Vector::float2 Position;
        Vector::float3 Color;
        Vector::float2 TexCoord;
        
        static VkVertexInputBindingDescription BindingDescription()
        {
            VkVertexInputBindingDescription description = {};
            
            description.binding = 0;
            description.stride = sizeof(Vertex);
            description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return description;
        }
        
        static VkVertexInputAttributeDescription PositionDescription()
        {
            VkVertexInputAttributeDescription description = {};
            
            description.binding = 0;
            description.location = 0;
            description.format = VK_FORMAT_R32G32_SFLOAT;
            description.offset = offsetof(Vertex, Position);
            
            return description;
        }

        static VkVertexInputAttributeDescription ColorDescription()
        {
            VkVertexInputAttributeDescription description = {};
            
            description.binding = 0;
            description.location = 1;
            //VK_FORMAT_R32G32B32A32_SFLOAT
            description.format = VK_FORMAT_R32G32B32_SFLOAT;
            description.offset = offsetof(Vertex, Color);
            
            return description;
        }
        
        static VkVertexInputAttributeDescription TextureDescription()
        {
            VkVertexInputAttributeDescription description = {};

            description.binding = 0;
            description.location = 2;
            description.format = VK_FORMAT_R32G32_SFLOAT;
            description.offset = offsetof(Vertex, TexCoord);
            
            return description;
        }

    };
    
    struct QueueFamilyConfig
    {
        bool GraphicsAvailable;
        int GraphicsIndex;
        
        bool PresentAvailable;
        int PresentIndex;
    };

    struct SwapchainConfig
    {
        static const int MAX_SIZE = 10; // This is referenced when creating arrays
        
        VkSurfaceCapabilitiesKHR Capabilities;
        VkSurfaceFormatKHR SurfaceFormat;
        VkPresentModeKHR PresentMode;
        VkExtent2D Extent;
        int Size;
    };

    struct VulkanConfig
    {
        int ValidationLayersCount;
        char** ValidationLayers;

        int VulkanExtensionsCount;
        char** VulkanExtensions;
        
        int DeviceExtensionsCount;
        char** DeviceExtensions;
    };

    struct Buffer
    {
        VkBuffer Handle;
        uint64 Size;
        VkDeviceMemory Memory;
        uint64 Offset;
    };

    struct Image
    {
        VkImage Handle;
        uint64 Size;
        VkDeviceMemory Memory;
        uint64 Offset;
    };
}
}
#endif