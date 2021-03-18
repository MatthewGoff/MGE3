#include <windows.h>

//typedef unsigned char byte;

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
//static BITMAPINFO bitmap_info;
//static void* bitmap_memory;
//static int bitmap_width;
//static int bitmap_height;
static int animation_offset;

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

    switch(uMsg)
    {
        //sent when windows is resized.
        case WM_SIZE:
        {
            Couple window_dimensions = GetWindowDimensions(hwnd);
            int width = window_dimensions.X;
            int height = window_dimensions.Y;
            ResizeDIBSection(width, height);
        } break;
        //"destory" sent when app is being closed
        //case WM_DESTROY:
        //"close" sent when windows is asking the app to close
        case WM_CLOSE:
        {
            running = false;
        } break;
        //sent when the app recieved focus
        case WM_ACTIVATEAPP:
        {
            
        } break;
        //windows requests to repaint for whatever reason
        case WM_PAINT:
        {
            Couple window_dimensions = GetWindowDimensions(hwnd);
            PAINTSTRUCT paint;
            HDC device_context = BeginPaint(hwnd, &paint);
            Win32UpdateWindow(device_context, window_dimensions);
            EndPaint(hwnd, &paint);
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
    }
    
    MessageBox(
        0,
        "this is handmade hero",
        "handmade hero",
        MB_OK|MB_ICONINFORMATION);

    return 0;
}