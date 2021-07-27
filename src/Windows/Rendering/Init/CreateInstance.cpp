#include <vulkan.h>
#include "..\Struct.h"

namespace MGE { namespace Windows { namespace Rendering { namespace Init
{
    bool SetupDebugMessenger(
        VkInstance vk_instance,
        VulkanConfig* config,
        VkDebugUtilsMessengerCreateInfoEXT* debug_info)
    {
        if (config->ValidationLayersCount == 0) return true;

        // Tutorial says we have to find this function at runtime.
        PFN_vkCreateDebugUtilsMessengerEXT func =
            (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
                vk_instance,
                "vkCreateDebugUtilsMessengerEXT");

        if (func == nullptr)
        {
            Error("[Error] Vulkan failed to load extension address.\n");
            return false;
        }

        VkDebugUtilsMessengerEXT debug_messenger; //We would store this if we wanted to conduct cleanup.
        VkResult result = func(
            vk_instance,
            debug_info,
            nullptr,
            &debug_messenger);

        if (result != VK_SUCCESS)
        {
            Error("[Error] Vulkan failed to create debug messenger.\n");
            return false;
        }

        return true;
    }

    bool ValidationLayersSupported(VulkanConfig* config)
    {
        uint32 count;
        vkEnumerateInstanceLayerProperties(&count, nullptr);

        Debug("[Debug] count = ");
        Debug(count);
        Debug("\n");
        
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

        for (int i = 0; i < config->ValidationLayersCount; i++)
        {
            char* desired_layer = (char*)config->ValidationLayers[i];
            bool layer_found = false;

            for (int j = 0; j < count; j++)
            {
                VkLayerProperties layer = available_layers[j];
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

    bool CreateInstance(
        VulkanConfig* config,
        VkDebugUtilsMessengerCreateInfoEXT* debug_info,
        VkInstance* vk_instance)
    {
        if (!ValidationLayersSupported(config))
        {
            Error("[Error] Missing support for validation layers.\n")
            return false;
        }
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
        
        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = config->ValidationLayersCount;
        createInfo.ppEnabledLayerNames = (char**)config->ValidationLayers;
        createInfo.enabledExtensionCount = config->VulkanExtensionsCount;
        createInfo.ppEnabledExtensionNames = (char**)config->VulkanExtensions;
        createInfo.pNext = debug_info;

        VkResult result = vkCreateInstance(
            &createInfo,
            nullptr,
            vk_instance);
        
        if (result != VK_SUCCESS)
        {
            Error("[Error] Vulkan failed to create instance.\n")
            return false;
        }
        
        bool success = SetupDebugMessenger(*vk_instance, config, debug_info);
        if (!success)
        {
            Error("[Error] Failed to setup debug messenger.\n");
            return false;
        }

        return true;
    }
}
}}}