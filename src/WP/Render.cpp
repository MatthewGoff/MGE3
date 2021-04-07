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

VkInstance VulkanInstance;

VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
VkDevice LogicalDevice;
VkQueue GraphicsQueue;
VkQueue PresentQueue;
VkSurfaceKHR TargetSurface;
VkDebugUtilsMessengerEXT DebugMessenger;

char* ValidationLayers[] = {"VK_LAYER_KHRONOS_validation"};
int ValidationLayersCount = 1;

char* Extensions[] = {
    "VK_KHR_surface",
    "VK_KHR_win32_surface",
    "VK_EXT_debug_utils",
    "VK_KHR_get_physical_device_properties2"};
int ExtensionsCount = 4;

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

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{

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

bool CheckValidationLayerSupport()
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
    VkLayerProperties layer_properties[20];
    vkEnumerateInstanceLayerProperties(&count, layer_properties);

    for (int i = 0; i < ValidationLayersCount; i++)
    {
        char* desired_layer = ValidationLayers[i];
        bool layerFound = false;

        for (int j = 0; j < count; j++)
        {
            VkLayerProperties properties = layer_properties[j];
            Print("layer name = ");
            Print(properties.layerName);
            Print("\n");
            if (String::Compare(desired_layer, properties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

bool CreateInstance()
{
    if (!CheckValidationLayerSupport())
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

bool SuitableDevice(VkPhysicalDevice PhysicalDevice)
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(PhysicalDevice, &deviceProperties);
    
    /*
    Print("Device name: ");
    Print(deviceProperties.deviceName);
    Print("\n");
    */
    
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(PhysicalDevice, &deviceFeatures);
    
    FindQueueFamilies(PhysicalDevice);
    
    if (QueueFamilies.GraphicsAvailable && QueueFamilies.PresentAvailable)
    {
        return true;
    }
    else
    {
        return false;
    }
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
    success = CreateLogicalDevice();
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