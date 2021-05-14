#ifndef INIT_H
#define INIT_H

#include "..\Struct.h"

namespace WindowsOS { namespace Rendering {
namespace Init
{
    bool CreateInstance(
        VkConfig* config,
        VkDebugUtilsMessengerCreateInfoEXT* debug_info,
        VkInstance* vulkan_handle);

    bool CreateSurface(
        VkInstance vulkan_handle,
        HINSTANCE instance_handle,
        HWND window_handle,
        VkSurfaceKHR* surface_handle);

    bool SelectPhysicalDevice(
        VkConfig* config,
        VkInstance vulkan_handle,
        VkSurfaceKHR surface_handle,
        QueueFamilies* queue_families,
        SwapchainSupport* swapchain_support,
        VkPhysicalDevice* physical_device_handle);

    bool CreateLogicalDevice(
        VkConfig* config,
        VkPhysicalDevice physical_device_handle,
        QueueFamilies* queue_families,    
        VkQueue* GraphicsQueue,
        VkQueue* PresentQueue,
        VkDevice* logical_device_handle);

    bool CreateSwapchain(
        VkDevice logical_device_handle,
        VkSurfaceKHR surface_handle,
        QueueFamilies* queue_families,
        SwapchainSupport* swapchain_support,
        SwapchainMeta* swapchain_meta,
        VkImageView* image_views,
        VkSwapchainKHR* swapchain_handle);

    bool CreatePipeline(
        VkDevice logical_device_handle,
        SwapchainMeta* swapchain_meta,
        VkRenderPass* render_pass_handle,
        VkPipeline* pipeline_handle);

    bool CreateCommandBuffers(
        VkDevice logical_device_handle,
        QueueFamilies* queue_families,
        VkPipeline pipeline_handle,
        SwapchainMeta* swapchain_meta,
        VkRenderPass render_pass_handle,
        VkImageView* image_views,
        VkCommandBuffer* command_buffers);

    bool CreateVKSemaphore(
        VkDevice logical_device_handle,
        VkSemaphore* semaphore_handle);

}
} }

#endif