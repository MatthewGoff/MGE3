#undef UINT32_MAX
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan.h>
#include <windows.h>

#include "WP.h"

struct 
{
    bool GraphicsAvailable = false;
    bool PresentAvailable = false;
    int GraphicsIndex;
    int PresentIndex;
} QueueFamilies;

struct
{
    VkSurfaceCapabilitiesKHR Capabilities;
    int FormatsCount;
    VkSurfaceFormatKHR Formats[20];
    int PresentModesCount;
    VkPresentModeKHR PresentModes[20];
} SwapChainSupport;

VkInstance VulkanInstance;

VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
VkDevice LogicalDevice;
VkQueue GraphicsQueue;
VkQueue PresentQueue;
VkSurfaceKHR TargetSurface;
VkDebugUtilsMessengerEXT DebugMessenger;
VkPipeline GraphicsPipeline;

VkRenderPass RenderPass;
VkPipelineLayout PipelineLayout;

VkFormat SwapChainImageFormat;
VkExtent2D SwapChainExtent;
int SwapChainSize;
VkImage SwapChainImages[20];
VkImageView SwapChainImageViews[20];

char* ValidationLayers[] = {"VK_LAYER_KHRONOS_validation"};
int ValidationLayersCount = 1;

char* Extensions[] = {
    "VK_KHR_surface",
    "VK_KHR_win32_surface",
    "VK_EXT_debug_utils"};
int ExtensionsCount = 3;

char* DeviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
int DeviceExtensionsCount = 1;

/*
Tutorial says we have to find this function at runtime. This redirect will just lookup the function each time it is needed.
*/
VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    PFN_vkCreateDebugUtilsMessengerEXT func;
    {
        func = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    }

    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

bool CreateImageViews()
{
    for (int i = 0; i < SwapChainSize; i++)
    {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = *(SwapChainImages + i);
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = SwapChainImageFormat;
        
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        
        VkResult result = vkCreateImageView(
            LogicalDevice,
            &createInfo,
            nullptr,
            SwapChainImageViews + i);
        if (result != VK_SUCCESS)
        {
            Print(__func__);
            Print(" failure: Call to Vulkan returned %d.\n", result);
            return false;
        }
    }
    
    return true;
}


/*
Return false if failed to complete or if swapchain support is inssuficient.
*/
bool DetermineSwapChainSupport(VkPhysicalDevice physical_device)
{
    SwapChainSupport = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physical_device,
        TargetSurface,
        &SwapChainSupport.Capabilities);

    uint32 count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device,
        TargetSurface,
        &count,
        nullptr);
    
    if (count == 0 || count > 20)
    {
        return false;
    }
    
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device,
        TargetSurface,
        &count,
        SwapChainSupport.Formats);
    SwapChainSupport.FormatsCount = count;

    count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device,
        TargetSurface,
        &count,
        nullptr);
    
    if (count == 0 || count > 20)
    {
        return false;
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device,
        TargetSurface,
        &count,
        SwapChainSupport.PresentModes);
    SwapChainSupport.PresentModesCount = count;

    /*
    Check for swapchain being addequate: (or another function)
    swapChainAdequate = !swapChainSupport.formats.empty() &&
    !swapChainSupport.presentModes.empty();
    */
    
    return true;
}

VkResult CreateShaderModule(
    byte* code,
    int length,
    VkShaderModule* shader_module)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = length;
    createInfo.pCode = (uint32*)code;
    
    VkResult result = vkCreateShaderModule(LogicalDevice, &createInfo, nullptr,
    shader_module);
    
    return result;
}

bool CreateGraphicsPipeline()
{
    byte* vert_spv = (byte*)malloc(10 * MEGABYTES);
    byte* frag_spv = (byte*)malloc(10 * MEGABYTES);

    int vert_size = WP::ReadEntireFile(
        vert_spv,
        10 * MEGABYTES,
        "Assets\\Shaders\\vert.spv");
    int frag_size = WP::ReadEntireFile(
        frag_spv,
        10 * MEGABYTES,
        "Assets\\Shaders\\frag.spv");

    VkShaderModule vert_module;
    VkShaderModule frag_module;
    
    Print("vert_size = %d.\n", vert_size);
    Print("frag_size = %d.\n", frag_size);
    
    VkResult result;
    result = CreateShaderModule(vert_spv, vert_size, &vert_module);
    if (result != VK_SUCCESS) return false;

    result = CreateShaderModule(frag_spv, frag_size, &frag_module);
    if (result != VK_SUCCESS) return false;
    
    VkPipelineShaderStageCreateInfo vert_stage_info = {};
    vert_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_stage_info.module = vert_module;
    vert_stage_info.pName = "main";
    vert_stage_info.pSpecializationInfo = nullptr;
    
    VkPipelineShaderStageCreateInfo frag_stage_info = {};
    frag_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_stage_info.module = frag_module;
    frag_stage_info.pName = "main";
    frag_stage_info.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo shader_stages[] = {
        vert_stage_info,
        frag_stage_info};

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 0;
    vertex_input_info.pVertexBindingDescriptions = nullptr; // Optional
    vertex_input_info.vertexAttributeDescriptionCount = 0;
    vertex_input_info.pVertexAttributeDescriptions = nullptr; // Optional

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) SwapChainExtent.width;
    viewport.height = (float) SwapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = SwapChainExtent;

    VkPipelineViewportStateCreateInfo viewport_state = {};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.pViewports = &viewport;
    viewport_state.scissorCount = 1;
    viewport_state.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
    
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    VkPipelineDepthStencilStateCreateInfo depth_stencil = {}; // Not using
    
    VkPipelineColorBlendAttachmentState color_blend_attachment = {};
    color_blend_attachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

    // Typical alpha blending
    #if 0
    color_blend_attachment.blendEnable = VK_TRUE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
    #endif
    
    VkPipelineColorBlendStateCreateInfo color_blending = {};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE; //VK_TRUE. for alpha blend
    color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;
    color_blending.blendConstants[0] = 0.0f; // Optional
    color_blending.blendConstants[1] = 0.0f; // Optional
    color_blending.blendConstants[2] = 0.0f; // Optional
    color_blending.blendConstants[3] = 0.0f; // Optional

    VkPipelineDynamicStateCreateInfo dynamicState = {}; // Not using

    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 0; // Optional
    pipeline_layout_info.pSetLayouts = nullptr; // Optional
    pipeline_layout_info.pushConstantRangeCount = 0; // Optional
    pipeline_layout_info.pPushConstantRanges = nullptr; // Optional
    
    //VkResult result;
    result = vkCreatePipelineLayout(
        LogicalDevice,
        &pipeline_layout_info,
        nullptr,
        &PipelineLayout);
    if (result != VK_SUCCESS) return false;

    VkGraphicsPipelineCreateInfo pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stages;
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState = &multisampling;
    pipeline_info.pDepthStencilState = nullptr; // Optional
    pipeline_info.pColorBlendState = &color_blending;
    pipeline_info.pDynamicState = nullptr; // Optional
    pipeline_info.layout = PipelineLayout;
    pipeline_info.renderPass = RenderPass;
    pipeline_info.subpass = 0;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipeline_info.basePipelineIndex = -1; // Optional

    //VkResult result;
    result = vkCreateGraphicsPipelines(
        LogicalDevice,
        VK_NULL_HANDLE,
        1,
        &pipeline_info,
        nullptr,
        &GraphicsPipeline);
    if (result != VK_SUCCESS)
    {
        return false;
    }

    vkDestroyShaderModule(LogicalDevice, vert_module, nullptr);
    vkDestroyShaderModule(LogicalDevice, frag_module, nullptr);
    free(vert_spv);
    free(frag_spv);
    
    return true;
}

bool CreateRenderPass()
{
    VkAttachmentDescription color_attachment = {};
    color_attachment.format = SwapChainImageFormat;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    
    VkResult result = vkCreateRenderPass(
        LogicalDevice,
        &render_pass_info,
        nullptr,
        &RenderPass);
    if (result != VK_SUCCESS)
    {
        return false;
    }

    return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    Print("Validation layer message: ");
    Print((char*)pCallbackData->pMessage);
    Print("\n");

    // VK_TRUE indicates to stop the thread generating the validation error
    return VK_FALSE;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = DebugCallback;
}

bool SetupDebugMessenger()
{
    if (ValidationLayersCount == 0) return true;
    
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    VkResult result = CreateDebugUtilsMessengerEXT(
        VulkanInstance,
        &createInfo,
        nullptr,
        &DebugMessenger);

    if (result != VK_SUCCESS)
    {
        Print(__func__);
        Print(" failure: Call to Vulkan returned %d.\n", result);
        return false;
    }

    return true;
}

VkPresentModeKHR ChooseSwapPresentMode(
    VkPresentModeKHR* available_present_modes,
    int count)
{
    for (int i = 0; i < count; i++)
    {
        VkPresentModeKHR available_present_mode = *(available_present_modes + i);
        if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return available_present_mode;
        }
    }
    
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D ChooseSwapExtent(VkSurfaceCapabilitiesKHR* capabilities)
{
    if (capabilities->currentExtent.width != UINT32_MAX)
    {
        return capabilities->currentExtent;
    }
    else
    {
        VkExtent2D extent = {WINDOW_WIDTH, WINDOW_HEIGHT};
        
        extent.width = Util::Min(capabilities->maxImageExtent.width, extent.width);
        extent.width = Util::Max(capabilities->minImageExtent.width,
        extent.width);
        
        extent.height = Util::Min(capabilities->maxImageExtent.height,
        extent.height);
        extent.height = Util::Max(capabilities->minImageExtent.height,
        extent.height);
        
        return extent;
    }
}

VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
    VkSurfaceFormatKHR* available_formats,
    int count)
{
    Assert(count!=0);
    for (int i = 0; i < count; i++) {
        VkSurfaceFormatKHR* available_format = (available_formats + i);
        if (available_format->format == VK_FORMAT_B8G8R8A8_SRGB &&
            available_format->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return *available_format;
        }
    }
    
    // First element of array is returned by default
    return *available_formats;
}

bool CreateSwapChain(VkPhysicalDevice physical_device, VkDevice logical_device)
{
    DetermineSwapChainSupport(physical_device); // Redundant since we did this when determining device suitability.

    VkSurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(SwapChainSupport.Formats, SwapChainSupport.FormatsCount);
    VkPresentModeKHR present_mode = ChooseSwapPresentMode(SwapChainSupport.PresentModes, SwapChainSupport.PresentModesCount);
    VkExtent2D extent = ChooseSwapExtent(&SwapChainSupport.Capabilities);
    
    uint32 image_count = SwapChainSupport.Capabilities.minImageCount + 1;
    
    // 0 is a reserved value indicating there is no maximum
    if (SwapChainSupport.Capabilities.maxImageCount != 0)
    {
        image_count =
            Util::Min(image_count, SwapChainSupport.Capabilities.maxImageCount);
    }
    
    if (image_count > 20)
    {
        return false;
    }
    
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = TargetSurface;
    createInfo.minImageCount = image_count;
    createInfo.imageFormat = surface_format.format;
    createInfo.imageColorSpace = surface_format.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    uint32 indices[] = {
        (uint32)QueueFamilies.GraphicsIndex,
        (uint32)QueueFamilies.PresentIndex};
    
    if (QueueFamilies.GraphicsIndex != QueueFamilies.PresentIndex)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = indices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = SwapChainSupport.Capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = present_mode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    
    VkSwapchainKHR swapChain;
    VkResult result = vkCreateSwapchainKHR(
        logical_device,
        &createInfo,
        nullptr,
        &swapChain);
    if (result != VK_SUCCESS)
    {
        return false;
    }
    
    { // initialize swap chain images
        vkGetSwapchainImagesKHR(logical_device, swapChain, &image_count, nullptr);
        vkGetSwapchainImagesKHR(logical_device, swapChain, &image_count, SwapChainImages);
    }
    
    SwapChainImageFormat = surface_format.format;
    SwapChainExtent = extent;
    SwapChainSize = image_count;
    
    return true;
}


bool ValidationLayersSupported()
{
    uint32 count;
    vkEnumerateInstanceLayerProperties(&count, nullptr);

    Print("count = %d\n", (int64)count);
    if (count <= 0)
    {
        return false;
    }
    if (count > 20)
    {
        return false;
    }
    VkLayerProperties available_layers[20];
    vkEnumerateInstanceLayerProperties(&count, available_layers);

    for (int i = 0; i < ValidationLayersCount; i++)
    {
        char* desired_layer = ValidationLayers[i];
        bool layer_found = false;

        for (int j = 0; j < count; j++)
        {
            VkLayerProperties layer = available_layers[j];
            //Print("layer name = ");
            //Print(layer.layerName);
            //Print("\n");
            if (String::Compare(desired_layer, layer.layerName) == 0)
            {
                layer_found = true;
                break;
            }
        }

        if (!layer_found) {
            return false;
        }
    }

    return true;
}

bool CreateInstance()
{
    if (!ValidationLayersSupported())
    {
        Print(__func__);
        Print(" failure: Validation layers not supported.\n");
        return false;
    }
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    uint32 glfwExtensionCount = 0;
    const char** glfwExtensions;
    
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = ValidationLayersCount;
    createInfo.ppEnabledLayerNames = ValidationLayers;
    createInfo.enabledExtensionCount = ExtensionsCount;
    createInfo.ppEnabledExtensionNames = Extensions;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    populateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = &debugCreateInfo;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &VulkanInstance);
    
    if (result != VK_SUCCESS)
    {
        Print(__func__);
        Print(" failure: Call to Vulkan returned %d.\n", result);
        return false;
    }
    
    return true;
}

bool CreateSurface(HINSTANCE instance_handle, HWND window_handle)
{
    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = window_handle;
    createInfo.hinstance = instance_handle;
    
    VkResult result = vkCreateWin32SurfaceKHR(VulkanInstance, &createInfo, nullptr, &TargetSurface);
    if (result != VK_SUCCESS)
    {
        Print(__func__);
        Print(" failure: Call to Vulkan returned %d.\n", result);
        return false;
    }
    
    return true;
}

void FindQueueFamilies(VkPhysicalDevice physical_device)
{
    
    uint32 count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count,
    nullptr);
    
    if (count == 0)
    {
        Print(__func__);
        Print(" failure: No queue families found.\n");
        return;
    }
    if (count > 10)
    {
        Print(__func__);
        Print(" failure: More than len(buffer) queue families found.\n");
        return;
    }
    VkQueueFamilyProperties FamilyProperties[10];
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count,
    FamilyProperties);
    
    uint32 result;
    for (int i = 0; i < count; i++)
    {
        if (FamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            QueueFamilies.GraphicsAvailable = true;
            QueueFamilies.GraphicsIndex = i;
        }
        VkBool32 presentSupport;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, TargetSurface,
        &presentSupport);
        if (presentSupport)
        {
            QueueFamilies.PresentAvailable = true;
            QueueFamilies.PresentIndex = i;
        }
    }
}

bool DeviceSupportsExtensions(VkPhysicalDevice physical_device)
{    
    uint32 count;
    vkEnumerateDeviceExtensionProperties(
        physical_device,
        nullptr,
        &count,
        nullptr);

    if (count == 0)
    {
        Print(__func__);
        Print(" failure: No device extensions found.\n");
        return false;
    }
    if (count > 200)
    {
        Print(__func__);
        Print(" failure: More than len(buffer) device extensions found.\n");
        return false;
    }
    
    VkExtensionProperties available_extensions[200];
    vkEnumerateDeviceExtensionProperties(
        physical_device,
        nullptr,
        &count,
        available_extensions);

    for (int i = 0; i < DeviceExtensionsCount; i++)
    {
        char* desired_extension = DeviceExtensions[i];
        bool extension_found = false;

        for (int j = 0; j < count; j++)
        {
            VkExtensionProperties extension = available_extensions[j];
            if (String::Compare(desired_extension, extension.extensionName) == 0)
            {
                extension_found = true;
                break;
            }
        }

        if (!extension_found) {
            return false;
        }
    }

    return true;
}

bool SuitableDevice(VkPhysicalDevice physical_device)
{
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physical_device, &properties);

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(physical_device, &features);
    
    FindQueueFamilies(physical_device);
    
    if (!QueueFamilies.GraphicsAvailable || !QueueFamilies.PresentAvailable)
    {
        return false;
    }
    
    bool extension_support = DeviceSupportsExtensions(physical_device);
    if (!extension_support)
    {
        Print("No extension support.\n");
        return false;
    }

    bool swap_chain_support = DetermineSwapChainSupport(physical_device);
    if (!swap_chain_support)
    {
        Print("No swap chain support.\n");
        return false;
    }

    return true;
}

bool PickPhysicalDevice()
{
    uint32 count = 0;
    vkEnumeratePhysicalDevices(VulkanInstance, &count, nullptr);
    
    if (count == 0)
    {
        return false;
    }
    if (count > 10)
    {
        return false;
    }
    
    VkPhysicalDevice available_devices[10];
    vkEnumeratePhysicalDevices(VulkanInstance, &count, available_devices);
    
    for (int i = 0; i < count; i++)
    {
        if (available_devices[i] != nullptr && SuitableDevice(available_devices[i]))
        {
            PhysicalDevice = available_devices[i];
            return true;
        }
    }
    
    return false;
}

bool CreateLogicalDevice()
{
    VkDeviceQueueCreateInfo queueCreateInfo[2];
    {
        queueCreateInfo[0] = {};
        queueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo[0].queueFamilyIndex = QueueFamilies.GraphicsIndex;
        queueCreateInfo[0].queueCount = 1;
        
        queueCreateInfo[1] = {};
        queueCreateInfo[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo[1].queueFamilyIndex = QueueFamilies.PresentIndex;
        queueCreateInfo[1].queueCount = 1;
        
        float queuePriority = 1.0f;
        queueCreateInfo[0].pQueuePriorities = &queuePriority;
        queueCreateInfo[1].pQueuePriorities = &queuePriority;
    }
    
    //Print("GraphicsIndex = %d\n", QueueFamilies.GraphicsIndex);
    //Print("PresentIndex = %d\n", QueueFamilies.PresentIndex);
    
    VkPhysicalDeviceFeatures deviceFeatures = {};
    
    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledLayerCount = ValidationLayersCount;
    createInfo.ppEnabledLayerNames = ValidationLayers;
    createInfo.queueCreateInfoCount = 2;
    createInfo.pQueueCreateInfos = queueCreateInfo;
    createInfo.enabledExtensionCount = DeviceExtensionsCount;
    createInfo.ppEnabledExtensionNames = DeviceExtensions;

    VkResult result = vkCreateDevice(PhysicalDevice, &createInfo, nullptr, &LogicalDevice);
    if (result != VK_SUCCESS)
    {
        Print(__func__);
        Print(" failure: Failed to create logical device: vkresult = %d.\n", (int64)result);
        return false;
    }
    
    // Spec says they are void return
    vkGetDeviceQueue(LogicalDevice, QueueFamilies.GraphicsIndex, 0, &GraphicsQueue);
    vkGetDeviceQueue(LogicalDevice, QueueFamilies.PresentIndex, 0,
    &PresentQueue);
    
    return true;
}

bool initVulkan(HINSTANCE instance_handle, HWND window_handle)
{
    bool success;
    success = CreateInstance();
    if (!success)
    {
        Print(__func__);
        Print(" failure: Failed to create instance.\n");
        return false;
    }
    success = SetupDebugMessenger();
    if (!success)
    {
        Print(__func__);
        Print(" failure: Failed to setup debug messenger.\n");
        return false;
    }
    success = CreateSurface(instance_handle, window_handle);
    if (!success)
    {
        Print(__func__);
        Print(" failure: Failed to create surface.\n");
        return false;
    }
    success = PickPhysicalDevice();
    if (!success)
    {
        Print(__func__);
        Print(" failure: Failed to pick physical device.\n");
        return false;
    }
    success = CreateLogicalDevice();
    if (!success)
    {
        Print(__func__);
        Print(" failure: Failed to create logical device.\n");
        return false;
    }
    success = CreateSwapChain(PhysicalDevice, LogicalDevice);
    if (!success)
    {
        Print(__func__);
        Print(" failure: Failed to create swap chain.\n");
        return false;
    }
    success = CreateImageViews();
    if (!success)
    {
        Print(__func__);
        Print(" failure: Failed to create image views.\n");
        return false;
    }
    success = CreateRenderPass();
    if (!success)
    {
        Print(__func__);
        Print(" failure: Failed to create render pass.\n");
        return false;
    }
    success = CreateGraphicsPipeline();
    if (!success)
    {
        Print(__func__);
        Print(" failure: Failed to create graphics pipeline.\n");
        return false;
    }
    
    Print("Finished Vulkan initialization.\n");
    return true;
}

#if 0
void cleanup()
{
    vkDestroyInstance(VulkanInstance, nullptr);
}
#endif

#if 0
void VKDemo()
{
    uint32 count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &count,
    nullptr);
    
    Print("%d extensions supported\n", (int64)count);
    
    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;
}
#endif