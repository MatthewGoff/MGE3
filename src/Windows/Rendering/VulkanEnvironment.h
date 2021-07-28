#pragma once

#include "Struct.h"
#include "Device.h"
#include "Texture.h"

namespace MGE { namespace Windows { namespace Rendering {
    
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
    
    Texture MyTexture;
   
    // Objects with one instance per swapchain image
    VkImage Images[SwapchainConfig::MAX_SIZE];
    VkImageView ImageViews[SwapchainConfig::MAX_SIZE];
    VkFramebuffer Framebuffers[SwapchainConfig::MAX_SIZE];
    VkCommandBuffer CommandBuffers[SwapchainConfig::MAX_SIZE];
};

}}}