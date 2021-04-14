#ifndef RENDER_H
#define RENDER_H

namespace WindowsOS {
namespace Render
{
    bool initVulkan(HINSTANCE instance_handle, HWND window_handle);
    bool DrawFrame();
}
}

#endif