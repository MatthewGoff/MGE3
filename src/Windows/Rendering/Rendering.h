#ifndef RENDERING_H
#define RENDERING_H

namespace MGE { namespace Windows { namespace Rendering
{
    bool InitVulkan(HINSTANCE instance_handle, HWND window_handle);
    bool DrawFrame();
}}}

#endif