#ifndef STRUCT_H
#define STRUCT_H

#include <vulkan.h>

namespace WindowsOS {
namespace Rendering
{

    struct QueueFamilies
    {
        bool GraphicsAvailable = false;
        bool PresentAvailable = false;
        int GraphicsIndex;
        int PresentIndex;
    };

    struct SwapchainSupport
    {
        VkSurfaceCapabilitiesKHR Capabilities;
        int FormatsCount;
        VkSurfaceFormatKHR Formats[20];
        int PresentModesCount;
        VkPresentModeKHR PresentModes[20];
    };

    struct SwapchainMeta
    {
        VkFormat Format;
        VkExtent2D Extent;
        int Count;
    };

    struct VkConfig
    {
        int ValidationLayersCount;
        //char ValidationLayers[5][128];
        char** ValidationLayers;

        int VulkanExtensionsCount;
        //char VulkanExtensions[5][128];
        char** VulkanExtensions;
        
        int DeviceExtensionsCount;
        //char DeviceExtensions[5][128];
        char** DeviceExtensions;
    };
}
}

#endif