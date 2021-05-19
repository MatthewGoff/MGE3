#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan.h>

//#include "..\WindowsOS.h" // included by vulkan with win32 definition
#include "Init\Init.h"
#include "Struct.h"

// todo: remove these src/Engine/ imports. Used for opening bitmap and bitmap struct
#include "Engine\Engine.h"

namespace WindowsOS {
namespace Rendering
{
    VulkanEnvironment Environment;

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
    }
    
    /*
    FindMemoryType:
    Find a type of memory (available on physical_device) which has the desired properties provided
    by param "properties"
    */
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
            if ((typeFilter & (1 << i))
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
        bool success = FindMemoryType(
            Environment.PhysicalDevice,
            mem_requirements.memoryTypeBits,
            properties,
            memory_type);
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
            {{0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}, // bottom right
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, // bottom left
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // top left
            
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // top left
            {{0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}, {1.0f, 0.0f}}, // top right
            {{0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}} // bottom right
        };
        
        int vertices_count = 6;
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
        memcpy(data, vertices, vertices_size);
        vkUnmapMemory(logical_device, *vertex_buffer_memory);

        return true;
    }

    bool CreateUniformBuffers(VulkanEnvironment* env)
    {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        for (int i = 0; i < Environment.SwapchainConfig.Size; i++)
        {
            bool success = CreateDeviceBuffer(
                env->LogicalDevice,
                bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                &env->UniformBuffers[i],
                &env->UniformBuffersMemory[i]);
            if (!success)
            {
                return false;
            }
        }
        
        return true;
    }

    bool CreateDescriptorSetLayout(VulkanEnvironment* env)
    {
        VkDescriptorSetLayoutBinding ubo_layout_binding = {};
        ubo_layout_binding.binding = 0;
        ubo_layout_binding.descriptorCount = 1;
        ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        ubo_layout_binding.pImmutableSamplers = nullptr; // Optional

        VkDescriptorSetLayoutBinding sampler_layout_binding = {};
        sampler_layout_binding.binding = 1;
        sampler_layout_binding.descriptorCount = 1;
        sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        sampler_layout_binding.pImmutableSamplers = nullptr; // Optional
        
        VkDescriptorSetLayoutBinding bindings[] = {ubo_layout_binding, sampler_layout_binding};
        VkDescriptorSetLayoutCreateInfo layout_info = {};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = 2;
        layout_info.pBindings = bindings;
        
        VkResult result = vkCreateDescriptorSetLayout(
            env->LogicalDevice,
            &layout_info,
            nullptr,
            &env->DescriptorSetLayout);
        if (result != VK_SUCCESS)
        {
            return false;
            // for reference: "failed to create descriptor set layout!"
        }

        return true;
    }

    bool CreateDescriptorPool(VulkanEnvironment* env)
    {
        
        VkDescriptorPoolSize pool_sizes[2];
        pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_sizes[0].descriptorCount = (uint32)Environment.SwapchainConfig.Size;
        pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        pool_sizes[1].descriptorCount = (uint32)Environment.SwapchainConfig.Size;

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = 2;
        pool_info.pPoolSizes = pool_sizes;
        pool_info.maxSets = (uint32)Environment.SwapchainConfig.Size;

        VkResult result = vkCreateDescriptorPool(
            env->LogicalDevice,
            &pool_info,
            nullptr,
            &env->DescriptorPool);
        
        if (result != VK_SUCCESS)
        {
            return false;
            // for reference: "failed to create descriptor pool!"
        }
        
        return true;
    }

    bool CreateCommandPool(
        VulkanEnvironment* env,
        QueueFamilyConfig* queue_family_config)
    {
        VkCommandPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.queueFamilyIndex = queue_family_config->GraphicsIndex;
        pool_info.flags = 0; // Optional
        
        VkResult result = vkCreateCommandPool(
            env->LogicalDevice,
            &pool_info,
            nullptr,
            &env->CommandPool);
        if (result != VK_SUCCESS) {
            return false;
        }

        return true;
    }
    
    bool CreateDescriptorSets(VulkanEnvironment* env)
    {
        VkDescriptorSetLayout layouts[20];
        for (int i = 0; i < 20; i++)
        {
            layouts[i] = env->DescriptorSetLayout;
        }
        
        VkDescriptorSetAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = env->DescriptorPool;
        alloc_info.descriptorSetCount = (uint32)Environment.SwapchainConfig.Size;
        alloc_info.pSetLayouts = layouts;

        VkResult result = vkAllocateDescriptorSets(
            env->LogicalDevice,
            &alloc_info,
            env->DescriptorSets);
        if (result != VK_SUCCESS) {
            return false;
            // for reference: "failed to allocate descriptor sets!"
        }

        for (int i = 0; i < Environment.SwapchainConfig.Size; i++) {
            VkDescriptorBufferInfo buffer_info = {};
            buffer_info.buffer = env->UniformBuffers[i];
            buffer_info.offset = 0;
            buffer_info.range = sizeof(UniformBufferObject);
            
            VkDescriptorImageInfo image_info = {};
            image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            image_info.imageView = env->TextureView;
            image_info.sampler = env->TextureSampler;

            VkWriteDescriptorSet descriptor_writes[2];
            
            descriptor_writes[0] = {};
            descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_writes[0].dstSet = env->DescriptorSets[i];
            descriptor_writes[0].dstBinding = 0;
            descriptor_writes[0].dstArrayElement = 0;
            descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptor_writes[0].descriptorCount = 1;
            descriptor_writes[0].pBufferInfo = &buffer_info;
            
            descriptor_writes[1] = {};
            descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_writes[1].dstSet = env->DescriptorSets[i];
            descriptor_writes[1].dstBinding = 1;
            descriptor_writes[1].dstArrayElement = 0;
            descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptor_writes[1].descriptorCount = 1;
            descriptor_writes[1].pImageInfo = &image_info;
            
            vkUpdateDescriptorSets(env->LogicalDevice, 2, descriptor_writes, 0, nullptr);
        }

        return true;
    }

    VkCommandBuffer BeginSingleTimeCommands()
    {
        VkCommandBufferAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool = Environment.CommandPool;
        alloc_info.commandBufferCount = 1;

        VkCommandBuffer command_buffer;
        vkAllocateCommandBuffers(Environment.LogicalDevice, &alloc_info, &command_buffer);

        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(command_buffer, &begin_info);

        return command_buffer;
    }

    void EndSingleTimeCommands(VkCommandBuffer command_buffer)
    {
        vkEndCommandBuffer(command_buffer);

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffer;

        vkQueueSubmit(Environment.GraphicsQueue, 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(Environment.GraphicsQueue);

        vkFreeCommandBuffers(
            Environment.LogicalDevice,
            Environment.CommandPool,
            1,
            &command_buffer);
    }

    // Change the layout of an image in gpu memory
    void TransitionImageLayout(
        VkImage image,
        VkFormat format,
        VkImageLayout oldLayout,
        VkImageLayout newLayout)
    {
        VkCommandBuffer commandBuffer = BeginSingleTimeCommands();
        
        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED
            && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            
            sourceStage = VK_PIPELINE_STAGE_HOST_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else
        {
            *(int*)0 = 0;
        }

        vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage,
        destinationStage,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier);

        EndSingleTimeCommands(commandBuffer);
    }
    // Used in tutorial to move data from staging buffers to device optimal buffers.
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize
    size)
    {
        VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

        VkBufferCopy copyRegion = {};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        EndSingleTimeCommands(commandBuffer);
    }

    bool CreateImage(VulkanEnvironment* env, Bitmap* bitmap)
    {
        VkImageCreateInfo image_info = {};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VK_IMAGE_TYPE_2D;
        image_info.extent.width = (uint32)bitmap->Width;
        image_info.extent.height = (uint32)bitmap->Height;
        image_info.extent.depth = 1;
        image_info.mipLevels = 1;
        image_info.arrayLayers = 1;
        image_info.format = VK_FORMAT_R8G8B8A8_SRGB;
        image_info.tiling = VK_IMAGE_TILING_LINEAR;
        image_info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        image_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
        image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_info.flags = 0; // Optional

        VkResult result = vkCreateImage(env->LogicalDevice, &image_info, nullptr, &env->Texture);
        if (result != VK_SUCCESS)
        {
            return false;
            // for reference: "failed to create image"
        }
        
        VkMemoryRequirements memory_requirements;
        vkGetImageMemoryRequirements(env->LogicalDevice, env->Texture, &memory_requirements);
        
        uint32 memory_type;
        bool success = FindMemoryType(
            Environment.PhysicalDevice,
            memory_requirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            memory_type);
        if (!success)
        {
            return false;
        }
                    
        VkMemoryAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = memory_requirements.size;
        alloc_info.memoryTypeIndex = memory_type;
        
        result = vkAllocateMemory(
            env->LogicalDevice,
            &alloc_info,
            nullptr,
            &env->TextureMemory);
        if (result != VK_SUCCESS)
        {
            return false;
            // for reference: "failed to allocate image memory"
        }
        
        result = vkBindImageMemory(
            env->LogicalDevice,
            env->Texture,
            env->TextureMemory,
            0);
        
		void *data;
		vkMapMemory(env->LogicalDevice, env->TextureMemory, 0, memory_requirements.size, 0, &data);
		memcpy(data, bitmap->Pixels, memory_requirements.size);
		vkUnmapMemory(env->LogicalDevice, env->TextureMemory);

        TransitionImageLayout(
            env->Texture,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_PREINITIALIZED,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        
        VkImageViewCreateInfo view_info = {};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = env->Texture;
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = VK_FORMAT_R8G8B8A8_SRGB;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        result = vkCreateImageView(
            env->LogicalDevice,
            &view_info,
            nullptr,
            &env->TextureView);
        if (result != VK_SUCCESS)
        {
            return false;
            // for reference: "failed to create texture image view"
        }

        VkSamplerCreateInfo sampler_info = {};
        sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter = VK_FILTER_LINEAR;
        sampler_info.minFilter = VK_FILTER_LINEAR;
        sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.anisotropyEnable = VK_TRUE;
        sampler_info.maxAnisotropy = 16;
        sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_info.unnormalizedCoordinates = VK_FALSE;
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp = VK_COMPARE_OP_NEVER;
        sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.mipLodBias = 0.0f;
        sampler_info.minLod = 0.0f;
        sampler_info.maxLod = 0.0f;

        result = vkCreateSampler(
            env->LogicalDevice,
            &sampler_info,
            nullptr,
            &env->TextureSampler);
        if (result != VK_SUCCESS)
        {
            return false;
            // for reference: "failed to create texture sampler"
        }

        return true;
    }
    
    bool FunImage(VulkanEnvironment* env)
    {
        Engine::LoadAsset(1);
        Bitmap* bitmap = Engine::GetAsset(1);
        
        bool success = CreateImage(env, bitmap);
        if (!success)
        {
            return false;
        }
        
        return true;
    }

    bool initVulkan(HINSTANCE instance_handle, HWND window_handle)
    {
        VkDebugUtilsMessengerCreateInfoEXT debug_info; //Can forget after init
        GetDebugInfo(&debug_info);

        VulkanConfig config;
        GetConfig(&config);

        bool success;

        success = Init::CreateInstance(&config, &debug_info, &Environment.Instance);
        if (!success)
        {
            Error("[Error] Failed to create instance.\n");
            return false;
        }

        success = Init::CreateSurface(
            Environment.Instance,
            instance_handle,
            window_handle,
            &Environment.Surface);
        if (!success)
        {
            Error("[Error] Failed to create surface.\n");
            return false;
        }

        success = Init::SelectPhysicalDevice(
            &config,
            Environment.Instance,
            Environment.Surface,
            &Environment.QueueFamilyConfig,
            &Environment.SwapchainConfig,
            &Environment.PhysicalDevice);
        if (!success)
        {
            Error("[Error] Failed to select physical device.\n");
            return false;
        }

        success = Init::CreateLogicalDevice(
            &config,
            Environment.PhysicalDevice,
            &Environment.QueueFamilyConfig,    
            &Environment.GraphicsQueue,
            &Environment.PresentQueue,
            &Environment.LogicalDevice);
        if (!success)
        {
            Error("[Error] Failed to create logical device.\n");
            return false;
        }

        success = Init::CreateSwapchain(
            &Environment,
            &Environment.QueueFamilyConfig,
            &Environment.SwapchainConfig);
        if (!success)
        {
            Error("[Error] Failed to create swap chain.\n");
            return false;
        }

        success = CreateDescriptorSetLayout(&Environment);
        if (!success)
        {
            Error("[Error] Failed to create descriptor set layout.\n");
            return false;
        }

        success = Init::CreatePipeline(
            &Environment,
            Environment.LogicalDevice,
            &Environment.SwapchainConfig,
            &Environment.RenderPass,
            &Environment.Pipeline,
            &Environment.DescriptorSetLayout);
        if (!success)
        {
            Error("[Error] Failed to create graphics pipeline.\n");
            return false;
        }
        
        success = CreateCommandPool(&Environment, &Environment.QueueFamilyConfig);
        if (!success)
        {
            Error("[Error] Failed to create command pool.\n");
            return false;
        }

        success = FunImage(&Environment);
        if (!success)
        {
            Error("[Error] Failed to create Image.\n");
            return false;
        }
        
        success = CreateVertexBuffer(
            Environment.LogicalDevice,
            &Environment.VertexBuffer,
            &Environment.VertexBufferMemory);
        if (!success)
        {
            Error("[Error] Failed to create vertex buffer.\n");
            return false;
        }

        success = CreateUniformBuffers(&Environment);
        if (!success)
        {
            Error("[Error] Failed to create uniform buffers.\n");
            return false;
        }

        success = CreateDescriptorPool(&Environment);
        if (!success)
        {
            Error("[Error] Failed to create descriptor pool.\n");
            return false;
        }

        success = CreateDescriptorSets(&Environment);
        if (!success)
        {
            Error("[Error] Failed to create descriptor sets.\n");
            return false;
        }

        success = Init::CreateCommandBuffers(
            &Environment,
            &Environment.QueueFamilyConfig,
            &Environment.SwapchainConfig);
        if (!success)
        {
            Error("[Error] Failed to create command buffers.\n");
            return false;
        }

        success = Init::CreateVKSemaphore(
            Environment.LogicalDevice,
            &Environment.ImageAvailableSemaphore);
        success = success && Init::CreateVKSemaphore(
            Environment.LogicalDevice,
            &Environment.RenderFinishedSemaphore);
        if (!success)
        {
            Error("[Error] Failed to create semaphores.\n");
            return false;
        }

        Info("[Info] Finished Vulkan initialization.\n");
        return true;
    }

    void UpdateUniformBuffer(uint32 currentImage)
    {
        UniformBufferObject ubo = {};

        ubo.CameraPosition = {0.0, 0.0};//{0.1, 0.1};
        ubo.CameraDimensions = {0.0, 0.0};//{0.1, 0.1};
        
        void* data;
        vkMapMemory(
            Environment.LogicalDevice,
            Environment.UniformBuffersMemory[currentImage],
            0,
            sizeof(ubo),
            0,
            &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(
            Environment.LogicalDevice,
            Environment.UniformBuffersMemory[currentImage]);

    }
    
    bool DrawFrame()
    {
        // Make sure asyncronous calls from the previous DrawFrame() have finished
        vkQueueWaitIdle(Environment.PresentQueue);
        
        uint32 image_index;
        VkResult result = vkAcquireNextImageKHR(
            Environment.LogicalDevice,
            Environment.Swapchain,
            UINT64_MAX,
            Environment.ImageAvailableSemaphore,
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

        UpdateUniformBuffer(image_index);

        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        
        VkSemaphore wait_semaphores[] = {Environment.ImageAvailableSemaphore};
        VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &Environment.CommandBuffers[image_index];

        VkSemaphore signal_semaphores[] = {Environment.RenderFinishedSemaphore};
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;

        //VkResult result;
        result = vkQueueSubmit(
            Environment.GraphicsQueue,
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
        VkSwapchainKHR swap_chains[] = {Environment.Swapchain};
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swap_chains;
        present_info.pImageIndices = &image_index;
        present_info.pResults = nullptr; // Optional

        vkQueuePresentKHR(Environment.PresentQueue, &present_info);
        vkQueueWaitIdle(Environment.PresentQueue);

        return true;
    }
}
}