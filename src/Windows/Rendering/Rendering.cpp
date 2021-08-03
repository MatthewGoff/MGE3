#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan.h>

//#include "..\Windows.h" // included by vulkan with win32 definition
#include "Init\Init.h"
#include "Struct.h"
#include "VulkanEnvironment.h"
#include "Texture.h"

// todo: remove these src/Engine/ imports. Used for opening bitmap and bitmap struct
#include "Engine\Engine.h"

namespace MGE { namespace Windows { namespace Rendering {

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
    uint image_index = 1;
    Vertex vertices[] =
    {
        {{0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, image_index}, // bottom right
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, image_index}, // bottom left
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, image_index}, // top left
        
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, image_index}, // top left
        {{0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, image_index}, // top right
        {{0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, image_index} // bottom right
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
    sampler_layout_binding.descriptorCount = env->TEXTURE_COUNT;
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
    pool_sizes[0] = {};
    pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_sizes[0].descriptorCount = 1;
    pool_sizes[1] = {};
    pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_sizes[1].descriptorCount = env->TEXTURE_COUNT;

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = 2;
    pool_info.pPoolSizes = pool_sizes;
    pool_info.maxSets = 1;

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

bool CreateCommandPool(VulkanEnvironment* env, QueueFamilyConfig* queue_family_config)
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
    
    /*
    VkDescriptorImageInfo image_info = {};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = env->MyTextureOne.TextureView;
    image_info.sampler = env->MyTextureOne.TextureSampler;
    */
    
    VkDescriptorImageInfo image_info[env->TEXTURE_COUNT];
    for (int i = 0; i < env->TEXTURE_COUNT; i++)
    {
        image_info[i] = {};
        image_info[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info[i].imageView = env->MyTextures[i].TextureView;
        image_info[i].sampler = env->MyTextures[i].TextureSampler;
    }

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
    descriptor_writes[1].descriptorCount = env->TEXTURE_COUNT;
    descriptor_writes[1].pImageInfo = image_info;

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

// Used in tutorial to move data from staging buffers to device optimal buffers.
void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

    VkBufferCopy copyRegion = {};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    EndSingleTimeCommands(commandBuffer);
}

bool FunImage(VulkanEnvironment* env)
{
    bool success;
    
    Engine::LoadAsset(1);
    Engine::LoadAsset(2);
    Bitmap* bitmap[2];
    bitmap[0] = Engine::GetAsset(1);
    bitmap[1] = Engine::GetAsset(2);
    
    for (int i = 0; i < env->TEXTURE_COUNT; i++)
    {
        int bitmap_index = 0;
        if (i == 9)
        {
            bitmap_index = 1;
        }
        success = env->MyTextures[i].Init(&env->Device, bitmap[i % 2]);
        if (!success) {return false;}
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

    success = Environment.Device.Init(
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
    vkUnmapMemory(Environment.Device.LogicalDevice, Environment.Device.UniformBuffer.Memory);

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
        Error("[Error] Swap chain out of date.\n");
        return false;
    }
    else if (result == VK_SUBOPTIMAL_KHR)
    {
        Warning("[Warning] Swap chain suboptimal.\n");
    }
    else if (result != VK_SUCCESS)
    {
        Error("[Error] Failed to acquire image buffer at top of draw frame.\n");
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