#include <vulkan.h>

namespace WindowsOS { namespace Rendering {
namespace Init
{
    bool CreateVKSemaphore(
        VkDevice logical_device_handle,
        VkSemaphore* semaphore_handle)
    {
        VkSemaphoreCreateInfo semaphore_info = {};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkResult result = vkCreateSemaphore(
            logical_device_handle,
            &semaphore_info,
            nullptr,
            semaphore_handle);
        if (result != VK_SUCCESS)
        {
            return false;
        }
        
        return true;
    }
}
} }