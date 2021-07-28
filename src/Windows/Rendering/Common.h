#pragma once
#include <vulkan.h>

namespace MGE { namespace Windows { namespace Rendering {

// Rendering.cpp
VkCommandBuffer BeginSingleTimeCommands();
void EndSingleTimeCommands(VkCommandBuffer command_buffer);

}}}