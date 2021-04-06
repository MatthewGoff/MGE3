//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include <vec4.hpp>
//#include <mat4x4.hpp>

#undef UINT32_MAX
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan.h>
#include <windows.h>

//#include <iostream>
//#include <stdexcept>
//#include <cstdlib>

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

bool CreateInstance()
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    char* validation_layers[] = {"VK_LAYER_KHRONOS_validation"};
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = 1;
    createInfo.ppEnabledLayerNames = validation_layers;

    {
        char* Extensions[] = {"VK_KHR_surface", "VK_KHR_win32_surface"};
        //const char* exention = "VK_KHR_surface";
        //Util::MoveString(*Extensions, &exention);
        createInfo.enabledExtensionCount = 2;
        createInfo.ppEnabledExtensionNames = Extensions;
    }

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
    
    uint32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queueFamilyCount,
    nullptr);
    
    if (queueFamilyCount == 0)
    {
        return;
    }
    if (queueFamilyCount > 10)
    {
        return;
    }
    VkQueueFamilyProperties FamilyProperties[10];
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queueFamilyCount,
    FamilyProperties);
    
    uint32 result;
    for (int i = 0; i < queueFamilyCount; i++)
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
    uint32 deviceCount = 0;
    vkEnumeratePhysicalDevices(VulkanInstance, &deviceCount, nullptr);
    
    Print("deviceCount = %d\n", (int64)deviceCount);
    if (deviceCount == 0)
    {
        return false;
    }
    if (deviceCount > 10)
    {
        return false;
    }
    
    VkPhysicalDevice available_devices[10];
    vkEnumeratePhysicalDevices(VulkanInstance, &deviceCount, available_devices);
    
    for (int i = 0; i < deviceCount; i++)
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
        queueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo[0].queueFamilyIndex = QueueFamilies.GraphicsIndex;
        queueCreateInfo[0].queueCount = 1;
        
        queueCreateInfo[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo[1].queueFamilyIndex = QueueFamilies.PresentIndex;
        queueCreateInfo[1].queueCount = 1;
        
        float queuePriority = 1.0f;
        queueCreateInfo[0].pQueuePriorities = &queuePriority;
        queueCreateInfo[1].pQueuePriorities = &queuePriority;
    }
    
    Print("GraphicsIndex = %d\n", QueueFamilies.GraphicsIndex);
    Print("PresentIndex = %d\n", QueueFamilies.PresentIndex);
    
    VkPhysicalDeviceFeatures deviceFeatures = {};
    
    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.queueCreateInfoCount = 2;
    createInfo.pQueueCreateInfos = queueCreateInfo;

    { // Compatability. We expect these fields to be ignored.
        char* Extensions[] = {"VK_KHR_surface", "VK_KHR_win32_surface"};
        //const char* exention = "VK_KHR_surface";
        //Util::MoveString(*Extensions, &exention);
        createInfo.enabledExtensionCount = 2;
        createInfo.ppEnabledExtensionNames = Extensions;
        //createInfo.enabledExtensionCount = 0;
    }

    VkResult result = vkCreateDevice(PhysicalDevice, &createInfo, nullptr, &LogicalDevice);
    if (result != VK_SUCCESS)
    {
        Print("Could not create logical device\n");
        Print("vkresult = %d\n", (int64)result);
        return false;
    }
    
    Print("Created Logical Device");
    
    // Spec says they are void return
    vkGetDeviceQueue(LogicalDevice, QueueFamilies.GraphicsIndex, 0, &GraphicsQueue);
    vkGetDeviceQueue(LogicalDevice, QueueFamilies.PresentIndex, 0,
    &PresentQueue);
    
    return true;
}

void initVulkan(HINSTANCE instance_handle, HWND window_handle)
{
    bool success;
    Print("one\n");
    success = CreateInstance();
    if (!success)
    {
        Print("one failure\n");
        return;
    }
    Print("two\n");
    success = CreateSurface(instance_handle, window_handle);
    if (!success)
    {
        Print("two failure\n");
        return;
    }
    Print("three\n");
    success = PickPhysicalDevice();
    if (!success)
    {
        Print("three failure\n");
        return;
    }
    Print("four\n");
    success = CreateLogicalDevice();
    if (!success)
    {
        Print("four failure\n");
        return;
    }
    Print("five\n");
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
    uint32 extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
    nullptr);
    
    Print("%d extensions supported\n", (int64)extensionCount);
    
    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;
}
#endif