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
    QueueFamilySupport QueueFamilySupport;
    VkPhysicalDevice hPhysicalDevice = VK_NULL_HANDLE; // remove this
    VkDevice hLogicalDevice;
    VkQueue GraphicsQueue;
    VkQueue PresentQueue;
    VkSwapchainKHR hSwapchain;
    SwapchainConfig SwapchainConfig;
    VkImageView ImageViews[20];
    VkRenderPass hRenderPass;
    VkPipeline hPipeline;
    VkCommandBuffer CommandBuffers[20];
    VkSemaphore ImageAvailableSemaphore;
    VkSemaphore RenderFinishedSemaphore;
    VkDescriptorSetLayout DescriptorSetLayout;
    
    VkBuffer VertexBuffer;
    VkDeviceMemory VertexBufferMemory;

    //VkBuffer StagingBuffer;
    //VkDeviceMemory StagingBufferMemory;

    VkBuffer UniformBuffers[2];
    VkDeviceMemory UniformBuffersMemory[2];

    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
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
        *debug_info = {};
        debug_info->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
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

    void GetConfig(VulkanConfig* config)
    {
        *config = {};
        
        config->ValidationLayersCount = 1;
        config->ValidationLayers = new char*[] {"VK_LAYER_KHRONOS_validation"};

        config->VulkanExtensionsCount = 3;
        config->VulkanExtensions = new char*[] {
            "VK_KHR_surface",
            "VK_KHR_win32_surface",
            "VK_EXT_debug_utils"
        };
        
        config->DeviceExtensionsCount = 1;
        config->DeviceExtensions = new char*[] {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        
        config->SwapchainLength = 2;
    }
    
    bool FindMemoryType(
        VkPhysicalDevice physical_device,
        uint32 typeFilter,
        VkMemoryPropertyFlags properties,
        uint32 &out)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physical_device, &memProperties);

        for (uint32 i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if (
                (typeFilter & (1 << i))
                && ((memProperties.memoryTypes[i].propertyFlags & properties) == properties))
            {
                out = i;
                return true;
            }
        }
        
        return false;
        //for reference: "failed to find suitable memory type!"
    }

    bool CreateDeviceBuffer(
        VkDevice logical_device,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer* buffer,
        VkDeviceMemory* buffer_memory)
    {
        VkBufferCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        create_info.size = size;
        create_info.usage = usage;
        create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        
        VkResult result = vkCreateBuffer(
            logical_device,
            &create_info,
            nullptr,
            buffer);
        if (result != VK_SUCCESS)
        {
            return false;
        }
        
        VkMemoryRequirements mem_requirements;
        vkGetBufferMemoryRequirements(
            logical_device,
            *buffer,
            &mem_requirements);
        
        uint32 memory_type;
        bool success = FindMemoryType(hPhysicalDevice, mem_requirements.memoryTypeBits, properties, memory_type);
        if (!success) {return false;}
        
        VkMemoryAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = memory_type;
        
        // for reference: vkFreeMemory(logical_device, buffer_memory, nullptr)
        result = vkAllocateMemory(
            logical_device,
            &alloc_info,
            nullptr,
            buffer_memory);
        if (result != VK_SUCCESS)
        {
            return false;
        }
        
        vkBindBufferMemory(logical_device, *buffer, *buffer_memory, 0);
        
        return true;
    }

    bool CreateVertexBuffer(
        VkDevice logical_device,
        VkBuffer* vertex_buffer,
        VkDeviceMemory* vertex_buffer_memory)
    {
        Vertex vertices[] =
        {
            {{-0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}},
            {{0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}}
        };
        
        int vertices_count = 3;
        int vertices_size = vertices_count * sizeof(vertices[0]);
                
        bool success = CreateDeviceBuffer(
            logical_device,
            vertices_size,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            vertex_buffer,
            vertex_buffer_memory);
        if (!success)
        {
            return false;
        }
        
        void* data;
        vkMapMemory(logical_device, *vertex_buffer_memory, 0, vertices_size, 0, &data);
        memcpy(data, vertices, (size_t) vertices_size);
        vkUnmapMemory(logical_device, *vertex_buffer_memory);

        return true;
    }

#if false
    bool CreateDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding ubo_layout_binding = {};
        ubo_layout_binding.binding = 0;
        ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        ubo_layout_binding.descriptorCount = 1;
        ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        ubo_layout_binding.pImmutableSamplers = nullptr; // Optional

        VkDescriptorSetLayoutCreateInfo layout_info = {};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = 1;
        layout_info.pBindings = &ubo_layout_binding;
        
        VkResult = vkCreateDescriptorSetLayout(device, &layout_info, nullptr, &DescriptorSetLayout);
        if (result != VK_SUCCESS)
        {
            return false;
            // for reference: "failed to create descriptor set layout!"
        }

        return true;
    }
#endif
#if false
    bool CreateImage()
    {
        bool Engine::OpenBitmap(byte* mem, int mem_size, Bitmap* destination, int dest_size, char* path)
        
        LoadAsset(1);
        int* pixels = GetAsset(1).Pixels;

        CreateDeviceBuffer(
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            StagingBuffer,
            StagingBufferMemory);

        byte* pixels;
        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(device, stagingBufferMemory);
        //Don’t forget to clean up the original pixel array now:
        stbi_image_free(pixels);

        return true;
    }
#endif
    bool initVulkan(HINSTANCE instance_handle, HWND window_handle)
    {
        VkDebugUtilsMessengerCreateInfoEXT debug_info; //Can forget after init
        GetDebugInfo(&debug_info);

        VulkanConfig config;
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
            &QueueFamilySupport,
            &SwapchainConfig,
            &hPhysicalDevice);
        if (!success)
        {
            Error("[Error] Failed to select physical device.\n");
            return false;
        }

        success = Init::CreateLogicalDevice(
            &config,
            hPhysicalDevice,
            &QueueFamilySupport,    
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
            &QueueFamilySupport,
            &SwapchainConfig,
            ImageViews,
            &hSwapchain);
        if (!success)
        {
            Error("[Error] Failed to create swap chain.\n");
            return false;
        }
#if false
        success = CreateDescriptorSetLayout();
        if (!success)
        {
            Error("[Error] Failed to create descriptor set layout.\n");
            return false;
        }
#endif
        success = Init::CreatePipeline(
            hLogicalDevice,
            &SwapchainConfig,
            &hRenderPass,
            &hPipeline,
            &DescriptorSetLayout);
        if (!success)
        {
            Error("[Error] Failed to create graphics pipeline.\n");
            return false;
        }
        
        success = CreateVertexBuffer(hLogicalDevice, &VertexBuffer, &VertexBufferMemory);
        if (!success)
        {
            Error("[Error] Failed to create vertex buffer");
            return false;
        }

        success = Init::CreateCommandBuffers(
            hLogicalDevice,
            &QueueFamilySupport,
            VertexBuffer,
            hPipeline,
            &SwapchainConfig,
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
#if false
        success = CreateImage()
        if (!success)
        {
            Error("[Error] Failed to create Image.\n");
            return false;
        }
#endif
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
        vkQueueWaitIdle(PresentQueue);

        return true;
    }
}
}