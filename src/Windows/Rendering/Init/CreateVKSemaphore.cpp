#include <vulkan.h>

namespace MGE { namespace Windows { namespace Rendering { namespace Init
{
    bool CreateVKSemaphore(
        VkDevice logical_device,
        VkSemaphore* semaphore)
    {
        VkSemaphoreCreateInfo semaphore_info = {};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkResult result = vkCreateSemaphore(
            logical_device,
            &semaphore_info,
            nullptr,
            semaphore);
        if (result != VK_SUCCESS)
        {
            return false;
        }
        
        return true;
    }
}
}}}