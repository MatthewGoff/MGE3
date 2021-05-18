#include "..\Struct.h"

namespace WindowsOS { namespace Rendering {
namespace Init
{
    VkFramebuffer Framebuffers[20];
    VkCommandPool CommandPool;
        
    bool CreateFramebuffers(
        VkDevice logical_device_handle,
        SwapchainConfig* swapchain_config,
        VkRenderPass render_pass_handle,
        VkImageView* image_views)
    {
        for (int i = 0; i < swapchain_config->Size; i++) {
            VkImageView attachments[] = {image_views[i]};

            VkFramebufferCreateInfo framebuffer_info = {};
            framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_info.renderPass = render_pass_handle;
            framebuffer_info.attachmentCount = 1;
            framebuffer_info.pAttachments = attachments;
            framebuffer_info.width = swapchain_config->Extent.width;
            framebuffer_info.height = swapchain_config->Extent.height;
            framebuffer_info.layers = 1;

            VkResult result = vkCreateFramebuffer(
                logical_device_handle,
                &framebuffer_info,
                nullptr,
                &Framebuffers[i]);
            if (result != VK_SUCCESS)
            {
                return false;
            }
        }
        
        return true;
    }
    
    bool CreateCommandPool(
        VkDevice logical_device_handle,
        QueueFamilySupport* queue_family_support)
    {
        VkCommandPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.queueFamilyIndex = queue_family_support->GraphicsIndex;
        pool_info.flags = 0; // Optional
        
        VkResult result = vkCreateCommandPool(
            logical_device_handle,
            &pool_info,
            nullptr,
            &CommandPool);
        if (result != VK_SUCCESS) {
            return false;
        }

        return true;
    }
        
    bool CreateCommandBuffers(
        VkDevice logical_device_handle,
        QueueFamilySupport* queue_family_support,
        VkBuffer vertex_buffer,
        VkPipeline pipeline_handle,
        SwapchainConfig* swapchain_config,
        VkRenderPass render_pass_handle,
        VkImageView* image_views,
        VkCommandBuffer* command_buffers)
    {
        bool success = CreateFramebuffers(
            logical_device_handle,
            swapchain_config,
            render_pass_handle,
            image_views);
        if (!success)
        {
            Error("[Error] Failed to create framebuffers.\n");
            return false;
        }

        success = CreateCommandPool(logical_device_handle, queue_family_support);
        if (!success)
        {
            Error("[Error] Failed to create command pool.\n");
            return false;
        }

        VkCommandBufferAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = CommandPool;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = swapchain_config->Size;

        VkResult result = vkAllocateCommandBuffers(
            logical_device_handle,
            &alloc_info,
            command_buffers);
        if (result != VK_SUCCESS)
        {
            return false;
        }
        
        for (int i = 0; i < swapchain_config->Size; i++)
        {
            VkCommandBufferBeginInfo begin_info = {};
            begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin_info.flags = 0; // Optional
            begin_info.pInheritanceInfo = nullptr; // Optional
            
            //VkResult result;
            result = vkBeginCommandBuffer(command_buffers[i], &begin_info);
            if (result != VK_SUCCESS) {
                return false;
            }
            
            VkRenderPassBeginInfo render_pass_info = {};
            render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            render_pass_info.renderPass = render_pass_handle;
            render_pass_info.framebuffer = Framebuffers[i];
            render_pass_info.renderArea.offset = {0, 0};
            render_pass_info.renderArea.extent = swapchain_config->Extent;
            VkClearValue clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
            render_pass_info.clearValueCount = 1;
            render_pass_info.pClearValues = &clear_color;
            
            vkCmdBeginRenderPass(
                command_buffers[i],
                &render_pass_info,
                VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(command_buffers[i],
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipeline_handle);
            
            VkBuffer buffers[] = {vertex_buffer};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(command_buffers[i], 0, 1, buffers, offsets);

            vkCmdDraw(command_buffers[i], 3, 1, 0, 0);
            
            vkCmdEndRenderPass(command_buffers[i]);
            
            VkResult result = vkEndCommandBuffer(command_buffers[i]);
            if (result != VK_SUCCESS)
            {
                return false;
            }

        }
        
        return true;
    }
}
} }