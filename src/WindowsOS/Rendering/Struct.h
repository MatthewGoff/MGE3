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
    };
    
    struct QueueFamilySupport
    {
        bool GraphicsAvailable;
        int GraphicsIndex;
        
        bool PresentAvailable;
        int PresentIndex;
    };

    struct SwapchainConfig
    {
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
        
        int SwapchainLength;
    };
#if false
    /*
    For objects with (one and only one) instance per swapchain image.
    */
    struct SwapGroup
    {
        VkImageView ImageView;
        VkCommandBuffer CommandBuffer;
        VkBuffer UniformBuffer;
        VkDeviceMemory UniformBufferMemory;
    };
    
    struct VulkanObjects
    {
        // These are, catagorically, opaque handles to the objects in question
        VkInstance Instance
        VkSurfaceKHR Surface;
        VkPhysicalDevice PhysicalDevice;
        VkDevice LogicalDevice;
        VkQueue GraphicsQueue;
        VkQueue PresentQueue;
        VkSwapchainKHR Swapchain;
        VkRenderPass RenderPass;
        VkPipeline Pipeline;
        VkSemaphore ImageAvailableSemaphore;
        VkSemaphore RenderFinishedSemaphore;
        VkDescriptorSetLayout DescriptorSetLayout;
        
        VkBuffer VertexBuffer;
        VkDeviceMemory VertexBufferMemory;        
        
        SwapGroup SwapGroup[];
        //QueueFamilies QueueFamilies;
        //SwapchainSupport SwapchainSupport;
        //SwapchainMeta SwapchainMeta;
    }
    
#endif
}
}
#endif