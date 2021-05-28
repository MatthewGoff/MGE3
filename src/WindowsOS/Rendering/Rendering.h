#ifndef RENDERING_H
#define RENDERING_H

namespace WindowsOS {
namespace Rendering
{
    bool InitVulkan(HINSTANCE instance_handle, HWND window_handle);
    bool DrawFrame();
}
}

#endif