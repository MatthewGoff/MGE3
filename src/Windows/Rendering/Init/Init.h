#ifndef INIT_H
#define INIT_H

#include "..\Struct.h"
#include "..\VulkanEnvironment.h"

namespace MGE { namespace Windows { namespace Rendering { namespace Init
{
    bool CreateInstance(
        VulkanConfig* config,
        VkDebugUtilsMessengerCreateInfoEXT* debug_info,
        VkInstance* vk_instance);

    bool CreateSurface(
        VkInstance vk_instance,
        HINSTANCE windows_instance,
        HWND window,
        VkSurfaceKHR* surface);

    bool SelectPhysicalDevice(
        VulkanConfig* config,
        VkInstance vk_instance,
        VkSurfaceKHR surface,
        QueueFamilyConfig* queue_family_config,
        SwapchainConfig* swapchain_config,
        VkPhysicalDevice* physical_device);
        
    bool CreateSwapchain(VulkanEnvironment* env);
        
    bool CreatePipeline(
        VulkanEnvironment* env,
        VkDescriptorSetLayout* descriptor_set_layouts);
        
    bool CreateCommandBuffers(VulkanEnvironment* env, VkBuffer vertex_buffer);

    bool CreateVKSemaphore(
        VkDevice logical_device,
        VkSemaphore* semaphore);

}
}}}

#endif