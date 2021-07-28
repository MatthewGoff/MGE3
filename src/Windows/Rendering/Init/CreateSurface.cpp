#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan.h>

namespace MGE { namespace Windows { namespace Rendering { namespace Init {

bool CreateSurface(
    VkInstance vk_instance,
    HINSTANCE windows_instance,
    HWND window,
    VkSurfaceKHR* surface)
{
    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = window;
    createInfo.hinstance = windows_instance;
    
    VkResult result = vkCreateWin32SurfaceKHR(
        vk_instance,
        &createInfo,
        nullptr,
        surface);
    if (result != VK_SUCCESS)
    {
        Error("[Error] Vulkan failed to create instance with result:")
        Error(result);
        Error("\n");
        
        return false;
    }
    
    return true;
}

}}}}