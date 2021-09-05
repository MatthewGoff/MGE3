#pragma once

namespace MGE { namespace Windows { namespace Rendering {

bool InitVulkan(HINSTANCE instance_handle, HWND window_handle, Bitmap* bitmap_one, Bitmap* bitmap_two);
bool DrawFrame();

}}}