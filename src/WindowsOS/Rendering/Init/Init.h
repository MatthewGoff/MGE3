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
        QueueFamilyConfig* queue_family_config,
        SwapchainConfig* swapchain_config,
        VkPhysicalDevice* physical_device_handle);

    bool CreateLogicalDevice(
        VulkanConfig* config,
        VkPhysicalDevice physical_device_handle,
        QueueFamilyConfig* queue_family_config,    
        VkQueue* GraphicsQueue,
        VkQueue* PresentQueue,
        VkDevice* logical_device_handle);
        
    bool CreateSwapchain(
        VulkanEnvironment* env,
        QueueFamilyConfig* queue_family_config,
        SwapchainConfig* swapchain_config);
        
    bool CreatePipeline(
        VulkanEnvironment* env,
        VkDevice logical_device_handle,
        SwapchainConfig* swapchain_config,
        VkRenderPass* render_pass_handle,
        VkPipeline* pipeline_handle,
        VkDescriptorSetLayout* descriptor_set_layout);
        
    bool CreateCommandBuffers(
        VulkanEnvironment* env,
        QueueFamilyConfig* queue_family_config,
        SwapchainConfig* swapchain_config);

    bool CreateVKSemaphore(
        VkDevice logical_device_handle,
        VkSemaphore* semaphore_handle);

}
} }

#endif