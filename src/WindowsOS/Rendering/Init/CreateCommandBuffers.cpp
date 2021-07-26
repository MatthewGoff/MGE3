#include "..\Struct.h"

namespace WindowsOS { namespace Rendering {
namespace Init
{        
    bool CreateFramebuffers(VulkanEnvironment* env)
    {
        for (int i = 0; i < env->SwapchainConfig.Size; i++) {
            VkImageView attachments[] = {env->ImageViews[i]};

            VkFramebufferCreateInfo framebuffer_info = {};
            framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_info.renderPass = env->RenderPass;
            framebuffer_info.attachmentCount = 1;
            framebuffer_info.pAttachments = attachments;
            framebuffer_info.width = env->SwapchainConfig.Extent.width;
            framebuffer_info.height = env->SwapchainConfig.Extent.height;
            framebuffer_info.layers = 1;

            VkResult result = vkCreateFramebuffer(
                env->Device.LogicalDevice,
                &framebuffer_info,
                nullptr,
                &env->Framebuffers[i]);
            if (result != VK_SUCCESS)
            {
                return false;
            }
        }
        
        return true;
    }
        
    bool CreateCommandBuffers(VulkanEnvironment* env, VkBuffer vertex_buffer)
    {
        bool success = CreateFramebuffers(env);
        if (!success)
        {
            Error("[Error] Failed to create framebuffers.\n");
            return false;
        }

        VkCommandBufferAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = env->CommandPool;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = env->SwapchainConfig.Size;

        VkResult result = vkAllocateCommandBuffers(
            env->Device.LogicalDevice,
            &alloc_info,
            env->CommandBuffers);
        if (result != VK_SUCCESS)
        {
            return false;
        }
        
        for (int i = 0; i < env->SwapchainConfig.Size; i++)
        {
            VkCommandBufferBeginInfo begin_info = {};
            begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin_info.flags = 0; // Optional
            begin_info.pInheritanceInfo = nullptr; // Optional
            
            //VkResult result;
            result = vkBeginCommandBuffer(env->CommandBuffers[i], &begin_info);
            if (result != VK_SUCCESS) {
                return false;
            }
            
            VkRenderPassBeginInfo render_pass_info = {};
            render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            render_pass_info.renderPass = env->RenderPass;
            render_pass_info.framebuffer = env->Framebuffers[i];
            render_pass_info.renderArea.offset = {0, 0};
            render_pass_info.renderArea.extent = env->SwapchainConfig.Extent;
            VkClearValue clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
            render_pass_info.clearValueCount = 1;
            render_pass_info.pClearValues = &clear_color;
            
            vkCmdBeginRenderPass(
                env->CommandBuffers[i],
                &render_pass_info,
                VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(
                env->CommandBuffers[i],
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                env->Pipeline);
            
            VkBuffer buffers[] = {vertex_buffer};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(env->CommandBuffers[i], 0, 1, buffers, offsets);

            vkCmdBindDescriptorSets(
                env->CommandBuffers[i],
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                env->PipelineLayout,
                0,
                1,
                &env->DescriptorSet,
                0,
                nullptr);

            vkCmdDraw(env->CommandBuffers[i], 6, 1, 0, 0);
            
            vkCmdEndRenderPass(env->CommandBuffers[i]);
            
            VkResult result = vkEndCommandBuffer(env->CommandBuffers[i]);
            if (result != VK_SUCCESS)
            {
                return false;
            }

        }
        
        return true;
    }
}
} }