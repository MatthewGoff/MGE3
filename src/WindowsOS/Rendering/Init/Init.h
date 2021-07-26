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
        
    bool CreateSwapchain(VulkanEnvironment* env);
        
    bool CreatePipeline(
        VulkanEnvironment* env,
        VkDescriptorSetLayout* descriptor_set_layouts);
        
    bool CreateCommandBuffers(VulkanEnvironment* env, VkBuffer vertex_buffer);

    bool CreateVKSemaphore(
        VkDevice logical_device_handle,
        VkSemaphore* semaphore_handle);

}
} }

#endif