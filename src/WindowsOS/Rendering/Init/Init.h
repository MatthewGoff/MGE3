#ifndef INIT_H
#define INIT_H

#include "..\Struct.h"

namespace WindowsOS { namespace Rendering {
namespace Init
{
    bool CreateInstance(
        VulkanConfig* config,
        VkDebugUtilsMessengerCreateInfoEXT* debug_info,
        VkInstance* vulkan_handle);

    bool CreateSurface(
        VkInstance vulkan_handle,
        HINSTANCE instance_handle,
        HWND window_handle,
        VkSurfaceKHR* surface_handle);

    bool SelectPhysicalDevice(
        VulkanConfig* config,
        VkInstance vulkan_handle,
        VkSurfaceKHR surface_handle,
        QueueFamilySupport* queue_family_support,
        SwapchainConfig* swapchain_config,
        VkPhysicalDevice* physical_device_handle);

    bool CreateLogicalDevice(
        VulkanConfig* config,
        VkPhysicalDevice physical_device_handle,
        QueueFamilySupport* queue_family_support,    
        VkQueue* GraphicsQueue,
        VkQueue* PresentQueue,
        VkDevice* logical_device_handle);

    bool CreateSwapchain(
        VkDevice logical_device_handle,
        VkSurfaceKHR surface_handle,
        QueueFamilySupport* queue_family_support,
        SwapchainConfig* swapchain_config,
        VkImageView* image_views,
        VkSwapchainKHR* swapchain_handle);

    bool CreatePipeline(
        VkDevice logical_device_handle,
        SwapchainConfig* swapchain_config,
        VkRenderPass* render_pass_handle,
        VkPipeline* pipeline_handle);

    bool CreateCommandBuffers(
        VkDevice logical_device_handle,
        QueueFamilySupport* queue_family_support,
        VkPipeline pipeline_handle,
        SwapchainConfig* swapchain_config,
        VkRenderPass render_pass_handle,
        VkImageView* image_views,
        VkCommandBuffer* command_buffers);

    bool CreateVKSemaphore(
        VkDevice logical_device_handle,
        VkSemaphore* semaphore_handle);

}
} }

#endif