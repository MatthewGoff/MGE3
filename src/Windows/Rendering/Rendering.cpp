#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan.h>

//#include "..\Windows.h" // included by vulkan with win32 definition
#include "Init\Init.h"
#include "Struct.h"
#include "VulkanEnvironment.h"

// todo: remove these src/Engine/ imports. Used for opening bitmap and bitmap struct
#include "Engine\Engine.h"

namespace MGE { namespace Windows { namespace Rendering
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

    bool CreateVertexBuffer(
        Device* device,
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
        
        void* data;
        vkMapMemory(device->LogicalDevice, *vertex_buffer_memory, 0, vertices_size, 0, &data);
        memcpy(data, vertices, vertices_size);
        vkUnmapMemory(device->LogicalDevice, *vertex_buffer_memory);

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
            env->Device.LogicalDevice,
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
            env->Device.LogicalDevice,
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
            env->Device.LogicalDevice,
            &pool_info,
            nullptr,
            &env->CommandPool);
        if (result != VK_SUCCESS) {
            return false;
        }

        return true;
    }
    
    bool CreateDescriptorSet(VulkanEnvironment* env)
    {        
        VkDescriptorSetAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = env->DescriptorPool;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &env->DescriptorSetLayout;

        VkResult result = vkAllocateDescriptorSets(
            env->Device.LogicalDevice,
            &alloc_info,
            &env->DescriptorSet);
        if (result != VK_SUCCESS) {
            return false;
            // for reference: "failed to allocate descriptor sets!"
        }

        VkDescriptorBufferInfo buffer_info = {};
        buffer_info.buffer = Environment.Device.UniformBuffer.Handle;
        buffer_info.offset = 0;
        buffer_info.range = sizeof(UniformBufferObject);
        
        VkDescriptorImageInfo image_info = {};
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info.imageView = env->TextureView;
        image_info.sampler = env->TextureSampler;

        VkWriteDescriptorSet descriptor_writes[2];
        
        descriptor_writes[0] = {};
        descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_writes[0].dstSet = env->DescriptorSet;
        descriptor_writes[0].dstBinding = 0;
        descriptor_writes[0].dstArrayElement = 0;
        descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_writes[0].descriptorCount = 1;
        descriptor_writes[0].pBufferInfo = &buffer_info;
        
        descriptor_writes[1] = {};
        descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_writes[1].dstSet = env->DescriptorSet;
        descriptor_writes[1].dstBinding = 1;
        descriptor_writes[1].dstArrayElement = 0;
        descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_writes[1].descriptorCount = 1;
        descriptor_writes[1].pImageInfo = &image_info;
        
        vkUpdateDescriptorSets(env->Device.LogicalDevice, 2, descriptor_writes, 0, nullptr);

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
        vkAllocateCommandBuffers(Environment.Device.LogicalDevice, &alloc_info, &command_buffer);

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
            Environment.Device.LogicalDevice,
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

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
            && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
            && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else
        {
            Error("[Error] Failed to change image format on graphics device.\n");
            return;
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
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
    {
        VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

        VkBufferCopy copyRegion = {};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        EndSingleTimeCommands(commandBuffer);
    }

    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32 width, uint32 height) {
        VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

        VkBufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {width, height, 1};

        vkCmdCopyBufferToImage(
            commandBuffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region);

        EndSingleTimeCommands(commandBuffer);
    }
    
    bool CreateImage(VulkanEnvironment* env, Bitmap* bitmap)
    {
        bool success;
        VkResult result;
        success = env->Device.CreateImage(
            bitmap->Width,
            bitmap->Height,
            Environment.Device.Texture.Size,
            &Environment.Device.Texture.Handle,
            &Environment.Device.Texture.Memory);

        // Part 2: Move memory to device
        
        VkMemoryRequirements mem_req;
        vkGetImageMemoryRequirements(env->Device.LogicalDevice, Environment.Device.Texture.Handle, &mem_req);    

        void* data;
        vkMapMemory(env->Device.LogicalDevice, Environment.Device.StagingBuffer.Memory, 0, mem_req.size, 0, &data);
        memcpy(data, bitmap->Pixels, (uint32)mem_req.size);
        vkUnmapMemory(env->Device.LogicalDevice, Environment.Device.StagingBuffer.Memory);

        TransitionImageLayout(
            Environment.Device.Texture.Handle,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        CopyBufferToImage(
            Environment.Device.StagingBuffer.Handle,
            Environment.Device.Texture.Handle,
            (uint32)bitmap->Width,
            (uint32)bitmap->Height);
        TransitionImageLayout(
            Environment.Device.Texture.Handle,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        
        // End part 2
        
        VkImageViewCreateInfo view_info = {};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = Environment.Device.Texture.Handle;
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = VK_FORMAT_R8G8B8A8_SRGB;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        result = vkCreateImageView(
            env->Device.LogicalDevice,
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
        sampler_info.maxAnisotropy = env->Device.PhysicalProperties.limits.maxSamplerAnisotropy;
        sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_info.unnormalizedCoordinates = VK_FALSE;
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
        sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.mipLodBias = 0.0f;
        sampler_info.minLod = 0.0f;
        sampler_info.maxLod = 0.0f;

        result = vkCreateSampler(
            env->Device.LogicalDevice,
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

    bool InitVulkan(HINSTANCE instance_handle, HWND window_handle)
    {
        VkDebugUtilsMessengerCreateInfoEXT debug_info; //Can forget after init
        GetDebugInfo(&debug_info);

        VulkanConfig config;

        bool success;

        Environment = {};
        
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

        VkPhysicalDevice physical_device;
        success = Init::SelectPhysicalDevice(
            &config,
            Environment.Instance,
            Environment.Surface,
            &Environment.QueueFamilyConfig,
            &Environment.SwapchainConfig,
            &physical_device);
        if (!success)
        {
            Error("[Error] Failed to select physical device.\n");
            return false;
        }

        success = Environment.Device.CreateDevice(
            &config,
            physical_device,
            &Environment.QueueFamilyConfig,    
            &Environment.GraphicsQueue,
            &Environment.PresentQueue);
        if (!success)
        {
            Error("[Error] Failed to create logical device.\n");
            return false;
        }

        success = Init::CreateSwapchain(
            &Environment);
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
            &Environment.Device,
            &Environment.Device.VertexBuffer.Handle,
            &Environment.Device.VertexBuffer.Memory);
        if (!success)
        {
            Error("[Error] Failed to create vertex buffer.\n");
            return false;
        }

        success = CreateDescriptorPool(&Environment);
        if (!success)
        {
            Error("[Error] Failed to create descriptor pool.\n");
            return false;
        }

        success = CreateDescriptorSet(&Environment);
        if (!success)
        {
            Error("[Error] Failed to create descriptor sets.\n");
            return false;
        }

        success = Init::CreateCommandBuffers(
            &Environment,
            Environment.Device.VertexBuffer.Handle);
        if (!success)
        {
            Error("[Error] Failed to create command buffers.\n");
            return false;
        }

        success = Init::CreateVKSemaphore(
            Environment.Device.LogicalDevice,
            &Environment.ImageAvailableSemaphore);
        success = success && Init::CreateVKSemaphore(
            Environment.Device.LogicalDevice,
            &Environment.RenderFinishedSemaphore);
        if (!success)
        {
            Error("[Error] Failed to create semaphores.\n");
            return false;
        }

        Info("[Info] Finished Vulkan initialization.\n");
        return true;
    }

    void UpdateUniformBuffer()
    {
        UniformBufferObject ubo = {};

        ubo.CameraPosition = {0.0, 0.0};//{0.1, 0.1};
        ubo.CameraDimensions = {19.2, 10.8};//{0.1, 0.1};
        
        void* data;
        vkMapMemory(
            Environment.Device.LogicalDevice,
            Environment.Device.UniformBuffer.Memory,
            0,
            sizeof(ubo),
            0,
            &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(
            Environment.Device.LogicalDevice,
            Environment.Device.UniformBuffer.Memory);

    }
    
    bool DrawFrame()
    {
        // Make sure asyncronous calls from the previous DrawFrame() have finished
        vkQueueWaitIdle(Environment.PresentQueue);
        
        uint32 image_index;
        VkResult result = vkAcquireNextImageKHR(
            Environment.Device.LogicalDevice,
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

        UpdateUniformBuffer();

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
}}}