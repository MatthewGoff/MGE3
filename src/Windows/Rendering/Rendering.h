#pragma once

namespace MGE { namespace Windows { namespace Rendering {

bool InitVulkan(HINSTANCE instance_handle, HWND window_handle);
bool DrawFrame();

}}}