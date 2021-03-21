#include <windows.h>
#include "Clock.h"
#include "Print.h"

// Generic couple of integers for bundling return values
struct Couple
{
    int X;
    int Y;
};

struct WinBitmap
{
    BITMAPINFO BitmapInfo;
    int Width;
    int Height;
    void* Bits;
};

//Global variable modified by windows callback when closing.
static bool running;

//other globals
static WinBitmap ScreenBuffer;
static int animation_offset;
static int InitialTime; // 32 bit value good for 277 hours.

/*
 * Returns width in X coord and height in Y coord
 */
static Couple
GetWindowDimensions(HWND window_handle)
{
    Couple result;
    
    HDC device_context = GetDC(window_handle);
    RECT client_rect;
    GetClientRect(window_handle, &client_rect);
    result.X = client_rect.right - client_rect.left;
    result.Y = client_rect.bottom - client_rect.top;
    
    return result;
}

static void
UpdateBitmap()
{
    for (int y = 0; y < ScreenBuffer.Height; y++)
    {
        for (int x = 0; x < ScreenBuffer.Width; x++)
        {
            int index = (y * ScreenBuffer.Width) + x;
            unsigned int* pixel_address = (unsigned int*) ScreenBuffer.Bits + index;
            
            int r = 0x00;
            int g = 0x00;
            int b = 0x00;
            int a = 0xFF; //alpha channel is only padding atm
            
            int y_offset = y + animation_offset;
            int x_offset = x + (2 * animation_offset);
            if (y_offset & (1 << 5))
            {
                r = 0xFF;
            }
            
            if (x_offset & (1 << 6))
            {
                b = 0xFF;
            }
            
            if ((x_offset + y_offset) & (1 << 7))
            {
                g = 0xFF;
            }
            
            // Windows specific ordering explained on handmade hero.
            int color = (a << (8 * 3)) + (r << (8 * 2)) + (g << (8 * 1)) + b;

            *pixel_address = color;
        }
    }
}

//(re)create our buffer (when window size changes)
static void
ResizeDIBSection(int width, int height)
{
    //need to allocate a new buffer every time because the size will be different.
    if (ScreenBuffer.Bits != NULL)
    {
        VirtualFree(ScreenBuffer.Bits, 0, MEM_RELEASE);
    }
    
    ScreenBuffer.Width = width;
    ScreenBuffer.Height = height;
    
    //standard values from handmade hero
    ScreenBuffer.BitmapInfo.bmiHeader.biSize = sizeof(ScreenBuffer.BitmapInfo.bmiHeader);
    ScreenBuffer.BitmapInfo.bmiHeader.biWidth = ScreenBuffer.Width;
    ScreenBuffer.BitmapInfo.bmiHeader.biHeight = -ScreenBuffer.Height; //negative to indicate top-down coordinate system
    ScreenBuffer.BitmapInfo.bmiHeader.biPlanes = 1;
    ScreenBuffer.BitmapInfo.bmiHeader.biBitCount = 32;
    ScreenBuffer.BitmapInfo.bmiHeader.biCompression = BI_RGB;
    
    int bytes_per_pixel = 4;
    int memory_size = bytes_per_pixel * ScreenBuffer.Width * ScreenBuffer.Height;
    ScreenBuffer.Bits = VirtualAlloc(0, memory_size, MEM_COMMIT, PAGE_READWRITE);
}

//Our own helper method for actually rendering
//...just resizes and forwards the bitmap
static void
Win32UpdateWindow(
    HDC device_context,
    Couple window_dimensions)
{
    StretchDIBits(
        device_context,
        0, 0, ScreenBuffer.Width, ScreenBuffer.Height,
        0, 0, window_dimensions.X, window_dimensions.Y,
        ScreenBuffer.Bits,
        &ScreenBuffer.BitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY);
}

//Handle windows events. Is given to windows as part of the window class which gets registered
LRESULT CALLBACK
WindowProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    LRESULT result = 0;

    switch (uMsg)
    {
        case WM_SIZE:
        {
            //sent when windows is resized.
            Couple window_dimensions = GetWindowDimensions(hwnd);
            int width = window_dimensions.X;
            int height = window_dimensions.Y;
            ResizeDIBSection(width, height);
        } break;
        case WM_DESTROY:
        {
            //"destory" sent when app is being closed. So this is redundant rly
            running = false;
        }
        case WM_CLOSE:
        {
            //"close" sent when windows is asking the app to close
            running = false;
        } break;
        case WM_ACTIVATEAPP:
        {
            //sent when the app recieved focus
        } break;
        case WM_PAINT:
        {
            //windows requests to repaint for whatever reason
            Couple window_dimensions = GetWindowDimensions(hwnd);
            PAINTSTRUCT paint;
            HDC device_context = BeginPaint(hwnd, &paint);
            Win32UpdateWindow(device_context, window_dimensions);
            EndPaint(hwnd, &paint);
        } break;
        // system keys like (alt-f4)
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        {
            // msdn: "Virtual-Key Codes"
            unsigned int vk_code = wParam;
            // bit 30 of lParam, "previous state", according to msdn 
            bool was_down = ((lParam & (1 << 30)) != 0);
            if (was_down)
            {
                break;
            }
            
            switch (vk_code)
            {
                case 'A':
                {
                } break;
                case 'S':
                {
                    
                } break;
                case 'D':
                {
                    
                } break;
                case 'F':
                {
                    
                } break;
                case VK_SPACE:
                {
                    int32 time = Clock::GetTimeMilli() - InitialTime;
                    Print("Current time: %f\n", time / 1000.0);
                }
            }
        } break;
        case WM_KEYUP:
        {
            
        } break;
        default:
        {
            result = DefWindowProc(hwnd, uMsg, wParam, lParam);
        } break;
    }
    
    return result;
}

int CALLBACK
WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    
    WNDCLASS window_class = {};
    // These two flags tell windows to redraw the whole window when it is resized. But we aren't going to be resizing the window.
    //window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = WindowProc;
    window_class.hInstance = hInstance;
    window_class.lpszClassName = "HandmadeHeroWindowClass";
    
    RegisterClass(&window_class);

    HWND window_handle = CreateWindowEx(
    0,
    window_class.lpszClassName,
    "Handmade Hero",
    WS_OVERLAPPEDWINDOW | WS_VISIBLE,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    0,
    0,
    hInstance,
    0);
    
    if (window_handle == NULL)
    {
        //then there was error setting up window
    }
    
    animation_offset = 0;

    Clock::Initialize();
    InitialTime = Clock::GetTimeMilli();
    
    int loop_time_stamp = Clock::GetTimeMicro();
    
    float target_framerate = 30.0; // hertz
    float target_frametime = 1000000.0 / target_framerate; // microseconds
    
    running = true;
    while(running)
    {
        MSG message;
        // Deal with all pending windows messages
        while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
        {        
            // You would think WM_QUIT translated and dispatched like all other
            // messages. It is a special case (see handmade hero and msdn) and I
            // don't know why.
            if (message.message == WM_QUIT)
            {
                running = false;
            }

            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        
        // render here
        animation_offset++;
        UpdateBitmap();

        HDC device_context = GetDC(window_handle);
        {
            Couple window_dimensions = GetWindowDimensions(window_handle);
            Win32UpdateWindow(device_context, window_dimensions);
        }
        ReleaseDC(window_handle, device_context);
        
        int current_stamp;
        int time_elapsed;
        do
        {
            current_stamp = Clock::GetTimeMicro();
            time_elapsed = current_stamp - loop_time_stamp;
        }
        while (time_elapsed < target_frametime);
        Print("Time elapsed = %u\n", time_elapsed);
        loop_time_stamp = current_stamp;
    }
    
    MessageBox(
        0,
        "this is handmade hero",
        "handmade hero",
        MB_OK|MB_ICONINFORMATION);

    return 0;
}