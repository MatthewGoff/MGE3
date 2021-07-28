#pragma once

#include "Struct.h"
#include "Device.h"

// todo: remove these src/Engine/ imports. Used for opening bitmap and bitmap struct
#include "Engine\Engine.h"

namespace MGE { namespace Windows { namespace Rendering {

class Texture
{
    public:
    
    bool Init(Device* device, Bitmap* bitmap);

    uint64 MemoryOffset;

    VkImage Image;
    VkImageView TextureView;
    VkSampler TextureSampler;

};

}}}