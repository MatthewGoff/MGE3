#undef UINT32_MAX
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan.h>
#include <windows.h>

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
    //swapChainImageViews.resize(swapChainImages.size());
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
            device,
            &createInfo,
            nullptr,
            swapChainImageViews + i)
        if (result != VK_SUCCESS)
        {
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

void CreateGraphicsPipeline()
{

}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    return false;
    Print("Validation layer message: ");
    Print((char*)pCallbackData->pMessage);
    Print("\n");

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
        Print("failed to set up debug messenger!\n");
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
        Print("Validation layer not supported\n");
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
    
    //glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    
    
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
        return;
    }
    if (count > 10)
    {
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

    Print("count of extensions = %d\n", (int64)count);
    if (count == 0)
    {
        return false;
    }
    if (count > 200)
    {
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
        //Print("desire extension name = ");
        //Print(desired_extension);
        //Print("\n");
        bool extension_found = false;

        for (int j = 0; j < count; j++)
        {
            VkExtensionProperties extension = available_extensions[j];
            //Print("extensionName name = ");
            //Print(extension.extensionName);
            //Print("\n");
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
    
    /*
    Print("Device name: ");
    Print(deviceProperties.deviceName);
    Print("\n");
    */
    
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
        Print("No extension support\n");
        return false;
    }

    bool swap_chain_support = DetermineSwapChainSupport(physical_device);
    if (!swap_chain_support)
    {
        Print("No swap chain support\n");
        return false;
    }

    return true;
}

bool PickPhysicalDevice()
{
    uint32 count = 0;
    vkEnumeratePhysicalDevices(VulkanInstance, &count, nullptr);
    
    Print("num physical devices = %d\n", (int64)count);
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
        Print("Failed to create logical device: vkresult = %d\n", (int64)result);
        return false;
    }
    
    // Spec says they are void return
    vkGetDeviceQueue(LogicalDevice, QueueFamilies.GraphicsIndex, 0, &GraphicsQueue);
    vkGetDeviceQueue(LogicalDevice, QueueFamilies.PresentIndex, 0,
    &PresentQueue);
    
    return true;
}

void initVulkan(HINSTANCE instance_handle, HWND window_handle)
{
    bool success;
    success = CreateInstance();
    success = SetupDebugMessenger();
    success = CreateSurface(instance_handle, window_handle);
    success = PickPhysicalDevice();
    if (!success)
    {
        Print("Failed to pick physical device\n");
        return;
    }
    success = CreateLogicalDevice();
    success = CreateSwapChain(PhysicalDevice, LogicalDevice);
    Print("Finished Vulkan initialization\n");
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