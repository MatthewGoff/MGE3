#ifndef VULKAN_ENVIRONMENT_H
#define VULKAN_ENVIRONMENT_H

#include "Struct.h"
#include "Device.h"

namespace WindowsOS {
namespace Rendering
{
    
struct VulkanEnvironment
{
    // Config
    QueueFamilyConfig QueueFamilyConfig;
    SwapchainConfig SwapchainConfig;
    
    // Vulkan objects (notice "Vk" prefix)
    Device Device;
    VkInstance Instance;
    VkSurfaceKHR Surface;
    VkQueue GraphicsQueue;
    VkQueue PresentQueue;
    VkSwapchainKHR Swapchain;
    VkRenderPass RenderPass;
    VkPipelineLayout PipelineLayout;
    VkPipeline Pipeline;
    VkCommandPool CommandPool;
    
    VkSemaphore ImageAvailableSemaphore;
    VkSemaphore RenderFinishedSemaphore;
    
    VkDescriptorSetLayout DescriptorSetLayout;
    VkDescriptorSet DescriptorSet;
    VkDescriptorPool DescriptorPool;
    
    VkImageView TextureView;
    VkSampler TextureSampler;
   
    // Objects with one instance per swapchain image
    VkImage Images[SwapchainConfig::MAX_SIZE];
    VkImageView ImageViews[SwapchainConfig::MAX_SIZE];
    VkFramebuffer Framebuffers[SwapchainConfig::MAX_SIZE];
    VkCommandBuffer CommandBuffers[SwapchainConfig::MAX_SIZE];
};
}}

#endif