#ifndef STRUCT_H
#define STRUCT_H

#include <vulkan.h>

namespace WindowsOS {
namespace Rendering
{

    struct QueueFamilySupport
    {
        bool GraphicsAvailable;
        int GraphicsIndex;
        
        bool PresentAvailable;
        int PresentIndex;
    };

    struct SwapchainConfig
    {
        VkSurfaceCapabilitiesKHR Capabilities;
        VkSurfaceFormatKHR SurfaceFormat;
        VkPresentModeKHR PresentMode;
        VkExtent2D Extent;
        int Size;
    };

    struct VulkanConfig
    {
        int ValidationLayersCount;
        char** ValidationLayers;

        int VulkanExtensionsCount;
        char** VulkanExtensions;
        
        int DeviceExtensionsCount;
        char** DeviceExtensions;
    };
}
}

#endif