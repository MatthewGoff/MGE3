#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan.h>

//#include "..\WindowsOS.h" // included by vulkan with win32 definition
#include "Init\Init.h"
#include "Struct.h"

namespace WindowsOS {
namespace Rendering
{
    
    VkInstance hVulkan = VK_NULL_HANDLE;
    VkSurfaceKHR hSurface;
    QueueFamilies QueueFamilies;
    VkPhysicalDevice hPhysicalDevice = VK_NULL_HANDLE; // remove this
    VkDevice hLogicalDevice;
    VkQueue GraphicsQueue;
    VkQueue PresentQueue;
    VkSwapchainKHR hSwapchain;
    SwapchainSupport SwapchainSupport;
    VkImageView ImageViews[20];
    SwapchainMeta SwapchainMeta;
    VkRenderPass hRenderPass;
    VkPipeline hPipeline;
    VkCommandBuffer CommandBuffers[20];
    VkSemaphore ImageAvailableSemaphore;
    VkSemaphore RenderFinishedSemaphore;

    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        /*
        Docs:
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT = 0x00000001,
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT = 0x00000010,
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT = 0x00000100,
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT = 0x00001000,
        */
        switch(messageSeverity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                Error("[Error] Validation layer: ");
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                Warning("[Warning] Validation layer: ");
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                goto RETURN;
                
                Info("[Info] Validation layer: ");
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                goto RETURN;
                
                Info("[Info] Validation layer: ");
                break;
        }
        
        Warning((char*)pCallbackData->pMessage);
        Warning("\n");

        RETURN:
        // VK_TRUE indicates to stop the thread generating the validation error
        return VK_FALSE;
    }

    void GetDebugInfo(VkDebugUtilsMessengerCreateInfoEXT* debug_info)
    {
        //true; // avoids editor fold bug
        *debug_info = {};
        debug_info->sType =
            VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debug_info->messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debug_info->messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debug_info->pfnUserCallback = DebugCallback;
    }

    void GetConfig(VkConfig* config)
    {
        //true; // avoids editor fold bug
        *config = {};
        
        config->ValidationLayersCount = 1;
        config->ValidationLayers = new char*[] {"VK_LAYER_KHRONOS_validation"};
        
        //Util::MoveString(
        //    config->ValidationLayers[0],
        //    "VK_LAYER_KHRONOS_validation");

        config->VulkanExtensionsCount = 3;
        config->VulkanExtensions = new char*[] {
            "VK_KHR_surface",
            "VK_KHR_win32_surface",
            "VK_EXT_debug_utils"
        };

        //Util::MoveString(config->VulkanExtensions[0], "VK_KHR_surface");
        //Util::MoveString(config->VulkanExtensions[1], "VK_KHR_win32_surface");
        ///Util::MoveString(config->VulkanExtensions[2], "VK_EXT_debug_utils");
        
        config->DeviceExtensionsCount = 1;
        config->DeviceExtensions = new char*[] {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        //Util::MoveString(
        //    config->DeviceExtensions[0],
        //    VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

    bool initVulkan(HINSTANCE instance_handle, HWND window_handle)
    {
        VkDebugUtilsMessengerCreateInfoEXT debug_info; //Can forget after init
        GetDebugInfo(&debug_info);

        VkConfig config;
        GetConfig(&config);

        bool success;

        success = Init::CreateInstance(&config, &debug_info, &hVulkan);
        if (!success)
        {
            Error("[Error] Failed to create instance.");
            return false;
        }

        success = Init::CreateSurface(
            hVulkan,
            instance_handle,
            window_handle,
            &hSurface);
        if (!success)
        {
            Error("[Error] Failed to create surface.\n");
            return false;
        }

        success = Init::SelectPhysicalDevice(
            &config,
            hVulkan,
            hSurface,
            &QueueFamilies,
            &SwapchainSupport,
            &hPhysicalDevice);
        if (!success)
        {
            Error("[Error] Failed to select physical device.\n");
            return false;
        }

        success = Init::CreateLogicalDevice(
            &config,
            hPhysicalDevice,
            &QueueFamilies,    
            &GraphicsQueue,
            &PresentQueue,
            &hLogicalDevice);
        if (!success)
        {
            Error("[Error] Failed to create logical device.\n");
            return false;
        }

        success = Init::CreateSwapchain(
            hLogicalDevice,
            hSurface,
            &QueueFamilies,
            &SwapchainSupport,
            &SwapchainMeta,
            ImageViews,
            &hSwapchain);
        if (!success)
        {
            Error("[Error] Failed to create swap chain.\n");
            return false;
        }

        success = Init::CreatePipeline(
            hLogicalDevice,
            &SwapchainMeta,
            &hRenderPass,
            &hPipeline);
        if (!success)
        {
            Error("[Error] Failed to create graphics pipeline.\n");
            return false;
        }
        
        success = Init::CreateCommandBuffers(
            hLogicalDevice,
            &QueueFamilies,
            hPipeline,
            &SwapchainMeta,
            hRenderPass,
            ImageViews,
            CommandBuffers);
        if (!success)
        {
            Error("[Error] Failed to create command buffers.\n");
            return false;
        }

        success = Init::CreateVKSemaphore(
            hLogicalDevice,
            &ImageAvailableSemaphore);
        success = success && Init::CreateVKSemaphore(
            hLogicalDevice,
            &RenderFinishedSemaphore);
        if (!success)
        {
            Error("[Error] Failed to create semaphores.\n");
            return false;
        }
        
        Info("[Info] Finished Vulkan initialization.\n");
        return true;
    }

    bool DrawFrame()
    {
        // Make sure asyncronous calls from the previous DrawFrame() have finished
        vkQueueWaitIdle(PresentQueue);
        
        uint32 image_index;
        VkResult result = vkAcquireNextImageKHR(
            hLogicalDevice,
            hSwapchain,
            UINT64_MAX,
            ImageAvailableSemaphore,
            VK_NULL_HANDLE,
            &image_index);
        
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            Error("[Error] Swap chain out of date");
            return false;
        }
        else if (result == VK_SUBOPTIMAL_KHR)
        {
            Warning("[Warning] Swap chain suboptimal");
        }
        else if (result != VK_SUCCESS)
        {
            Error("[Error] Failed to acquire image buffer at top of draw frame");
            return false;
        }

        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        
        VkSemaphore wait_semaphores[] = {ImageAvailableSemaphore};
        VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &CommandBuffers[image_index];

        VkSemaphore signal_semaphores[] = {RenderFinishedSemaphore};
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;

        //VkResult result;
        result = vkQueueSubmit(
            GraphicsQueue,
            1,
            &submit_info,
            VK_NULL_HANDLE);
        if (result != VK_SUCCESS)
        {
            return false;
        }
        
        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = signal_semaphores;
        VkSwapchainKHR swap_chains[] = {hSwapchain};
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swap_chains;
        present_info.pImageIndices = &image_index;
        present_info.pResults = nullptr; // Optional

        vkQueuePresentKHR(PresentQueue, &present_info);

        return true;
    }
}
}