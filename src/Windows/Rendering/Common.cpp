#include <vulkan.h>

namespace MGE { namespace Windows { namespace Rendering
{

uint64 MemoryAvailable(VkPhysicalDevice physical_device)
{
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);

    uint64 acc = 0;
    for (int i = 0; i < memory_properties.memoryHeapCount; i++)
    {
        acc += memory_properties.memoryHeaps[i].size;
    }
    return acc;
}

}}}