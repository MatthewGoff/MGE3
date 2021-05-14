#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan.h>

namespace WindowsOS { namespace Rendering {
namespace Init
{
    bool CreateSurface(
        VkInstance vulkan_handle,
        HINSTANCE instance_handle,
        HWND window_handle,
        VkSurfaceKHR* surface_handle)
    {
        VkWin32SurfaceCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = window_handle;
        createInfo.hinstance = instance_handle;
        
        VkResult result = vkCreateWin32SurfaceKHR(
            vulkan_handle,
            &createInfo,
            nullptr,
            surface_handle);
        if (result != VK_SUCCESS)
        {
            Error("[Error] Vulkan failed to create instance with result:")
            Error(result);
            Error("\n");
            
            return false;
        }
        
        return true;
    }
}
} }