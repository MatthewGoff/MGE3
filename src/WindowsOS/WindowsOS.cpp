#include <windows.h>

#include "Engine\Engine.h"
#include "Rendering\Rendering.h"
#include "Clock.h"

/* This file (and print and clock) contain all and only the code to abstract
 * the Windows operating system. To the best of my knowledge it is conventional
 * and as-you-would-expect. It is my own re-creation of the first 25
 * installments of the "Handemade Hero" tutorials.
*/
namespace WindowsOS
{
    static BITMAPINFO BitmapInfo;
    static bool running;
    static int InitialTime; // Milliseconds; 32 bit value good for 277 hours.

    /* We are looking for 4 gigabytes of memory. VirtualAlloc() will only take
     * a 32 bit value. 4 * 1024^3 happens to be the first number to overflow a
     * 32 bit uint. We pass UINT32_MAX and VirtualAlloc() will round up to the
     * nearest block length which will be exactly 4 gigabytes.
    */
    static uint32 RootMemorySize = (0xFFFFFFFF);//UINT32_MAX;
    
    void PostFatalMessage(char* message)
    {
        MessageBox(
            0,
            message,
            "Fatal Error",
            MB_OK|MB_ICONINFORMATION);
    }
    
    Vector::int2 GetWindowDimensions(HWND window_handle)
    {
        Vector::int2 result;
        
        HDC device_context = GetDC(window_handle);
        RECT client_rect;
        GetClientRect(window_handle, &client_rect);
        result.x = client_rect.right - client_rect.left;
        result.y = client_rect.bottom - client_rect.top;
        
        return result;
    }

    void PopulateBitmapInfo(int width, int height)
    {        
        //standard values from handmade hero
        BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
        BitmapInfo.bmiHeader.biWidth = width;
        BitmapInfo.bmiHeader.biHeight = -height; //negative to indicate top-down coordinate system
        BitmapInfo.bmiHeader.biPlanes = 1;
        BitmapInfo.bmiHeader.biBitCount = 32;
        BitmapInfo.bmiHeader.biCompression = BI_RGB;

    }

    //Our own helper method for actually rendering
    //...just resizes and forwards the bitmap
    void RefreshScreen(HWND window_handle, ScreenBuffer* ScreenBuffer)
    {
        HDC device_context = GetDC(window_handle);
        Vector::int2 window_dimensions = GetWindowDimensions(window_handle);
        StretchDIBits(
            device_context,
            0, 0, ScreenBuffer->Width, ScreenBuffer->Height,
            0, 0, window_dimensions.x, window_dimensions.y,
            &ScreenBuffer->Pixels,
            &BitmapInfo,
            DIB_RGB_COLORS,
            SRCCOPY);
        ReleaseDC(window_handle, device_context);
    }

    //Handle windows events. Is given to Windows as part of the window class which gets registered
    LRESULT CALLBACK WindowsCallback(
        HWND hwnd,
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam)
    {
        LRESULT result = 0;

        switch (uMsg)
        {
            // It's not neccesary to field WM_SIZE because we won't ever let the user resize the window
            #if 0
            case WM_SIZE:
            {
                //sent when windows is resized.
                Vector::int2 window_dimensions = GetWindowDimensions(hwnd);
                int width = window_dimensions.x;
                int height = window_dimensions.y;
                //PopulateBitmapInfo(width, height);
            } break;
            #endif
            case WM_DESTROY:
            {
                //"destory" sent when app is being closed. So this is redundant rly, idk
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
            // It's not neccesary to field WM_PAINT because the window will up updating at some fps anyways.
            #if 0
            case WM_PAINT: {} break;
            #endif
            default:
            {
                result = DefWindowProc(hwnd, uMsg, wParam, lParam);
            } break;
        }
        
        return result;
    }

    /* Process pending messages from the operating system.
     *
    */
    void ProcessMessages()
    {            
        MSG message;
        while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
        {
            switch (message.message)
            {
                /* You would think WM_QUIT translated and dispatched like all other messages. It is a special case (see handmade hero and msdn) and I don't know why. */
                if (message.message == WM_QUIT)
                {
                    running = false;
                }
                #if 0
                // system keys like (alt-f4)
                case WM_SYSKEYDOWN:
                case WM_SYSKEYUP:
                #endif
                case WM_KEYDOWN:
                {
                    // msdn: "Virtual-Key Codes"
                    unsigned int vk_code = message.wParam;
                    // bit 30 of lParam, "previous state", according to msdn 
                    bool was_down = ((message.lParam & (1 << 30)) != 0);
                    if (was_down)
                    {
                        break; // Discard all down events after the first
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
                            Info("[Info] Current time: ");
                            Info(time / 1000.0);
                            Info("\n");
                        }
                    }
                } break;
                case WM_KEYUP:
                {
                    
                } break;
                #if 0
                WM_LBUTTONDOWN
                WM_LBUTTONUP
                WM_RBUTTONDOWN
                WM_RBUTTONUP
                #endif
                default:
                {
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                } break;
            }
        }
    }
    
    // Returns number of bytes written to buffer. Returns 0 on failure.
    uint32 ReadEntireFile(void* buffer_ptr, uint32 buffer_size, char* file_name)
    {
        HANDLE file_handle = CreateFileA(
            file_name,
            GENERIC_READ, // We are only going to be reading
            FILE_SHARE_READ, // Don't allow other processes to write to this file
            nullptr,
            OPEN_EXISTING, // Fail if file not found.
            0,
            0);
        
        if (file_handle == INVALID_HANDLE_VALUE)
        {
            Error("[Error] Could not open file: ");
            Error(file_name);
            Error("\n");
            return 0;
        }
        
        LARGE_INTEGER file_size;
        // GetFileSize (without "ex") returns two 32 bit values.
        bool success = GetFileSizeEx(file_handle, &file_size);
        if (!success || file_size.QuadPart > buffer_size)
        {
            CloseHandle(file_handle);
            Error("[Error] File too large or could not determine size: ");
            Error(file_name);
            Error("\n");
            return 0;
        }

        //ReadFile() is restricted by UINT32_MAX
        Assert(file_size.QuadPart <= (0xFFFFFFFF));
        
        DWORD bytes_read;
        success = ReadFile(
            file_handle,
            buffer_ptr,
            (uint32) file_size.QuadPart,
            &bytes_read,
            0);
        
        CloseHandle(file_handle);
        
        if (!success)
        {            
            Error("[Error] OS failed to open file: ");
            Error(file_name);
            Error("\n");
            return 0;
        }
        
        return bytes_read;
    }
    
    bool GetFileSize(char* file_name, uint64* file_size)
    {
        HANDLE file_handle = CreateFileA(
            file_name,
            GENERIC_READ, // We are only going to be reading
            FILE_SHARE_READ, // Don't allow other processes to write to this file
            nullptr,
            OPEN_EXISTING, // Fail if file not found.
            0,
            0);

        if (file_handle == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        LARGE_INTEGER _file_size;
        // GetFileSize (without "ex") returns two 32 bit values.
        bool result = GetFileSizeEx(file_handle, &_file_size);

        CloseHandle(file_handle);

        if (result)
        {
            *file_size = (uint64)_file_size.QuadPart;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool WriteEntireFile(byte* buffer_ptr, uint32 buffer_size, char* file_name)
    {
        HANDLE file_handle = CreateFileA(
            file_name,
            GENERIC_WRITE,
            0, // no file sharing
            nullptr,
            CREATE_ALWAYS, // Overwrite; Create if missing
            0,
            0);
        
        if (file_handle == INVALID_HANDLE_VALUE)
        {
            return false;
        }
        
        DWORD bytes_written;
        bool result = WriteFile(
            file_handle,
            buffer_ptr,
            buffer_size,
            &bytes_written,
            0);
            
        CloseHandle(file_handle);
        if (!result || bytes_written != buffer_size)
        {
            return false;
        }
        
        return true;
    }
    
    void ProcessMouse(HWND window_handle, ControlInput* ControlInput)
    {
        POINT position;
        GetCursorPos(&position);
        
        ScreenToClient(window_handle, &position);
        ControlInput->CursorPosition = Vector::int2 {position.x, position.y};
        //Print("position = (%d, ", position.x);
        //Print("%d)\n", position.y);
    }

    // Runs once after initialization and before the main loop
    void Fluff(RootMemory* RootMemory)
    {
        uint32 bytes_read = ReadEntireFile(
            &RootMemory->FileBuffer,
            10 * MEGABYTES,
            "AppData\\my_data.txt");
        
        //Print("bytes_read = %d\n", (int64)bytes_read);
        
        uint64 file_size;
        GetFileSize("AppData\\my_data.txt", &file_size);
        //Print("\nFilesize = %d\n", (int64)file_size);
        
        char msg[] = "someone wants to know";
        byte* pointer = (byte*)&msg[0];
        
        bool success = WriteEntireFile(pointer, sizeof(msg)/sizeof(msg[0]), "AppData\\new_output.txt");
    }
    
    RootMemory* InitializeMemory()
    {
        // Actual physical pages are not allocated unless/until the virtual addresses are actually accessed. fyi
        
        // It is not clear to me why mem_reserve is neccesary on top of mem_commit but it appears to be conventional.
        RootMemory* memory = (RootMemory*)VirtualAlloc(0, RootMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        
        if (memory == nullptr)
        {
            return nullptr;
        }
        
        /* These test cases should be made to catch memory access errors.
         * Which ins't really possible without try/catch. I could try
         * compiling this separately to avoid the general overhead.
        */
        byte* beginning = (byte*)memory;
        byte* end = beginning + RootMemorySize;
        if (*beginning != 0x00 || *end != 0x00)
        {
            return nullptr;
        }
        
        *beginning = 0xFF;
        *end = 0xFF;

        if (*beginning != 0xFF || *end != 0xFF)
        {
            return nullptr;
        }
        
        *beginning = 0x00;
        *end = 0x00;
        
        return memory;
    }

    HWND InitializeWindow(HINSTANCE hInstance)
    {
        WNDCLASS window_class = {};
        // CS_HREDRAW and CS_VREDRAW tell windows to redraw the whole window when it is resized. But we aren't going to be resizing the window.
        //window_class.style = CS_HREDRAW | CS_VREDRAW;
        window_class.lpfnWndProc = WindowsCallback;
        window_class.hInstance = hInstance;
        window_class.lpszClassName = "MGEWindowClass";
        
        bool success = RegisterClass(&window_class);
        if (!success)
        {
            return nullptr;
        }

        /*
        style WS_OVERLAPPEDWINDOW ommited because it makes the window resizable
        style WS_SYSMENU provides the top right exit button.
        style WS_VISIBLE means the window is initiall visible.
        */
        HWND window_handle = CreateWindowEx(
        0,
        window_class.lpszClassName,
        "MGE",
        WS_SYSMENU | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0,
        0,
        hInstance,
        0);
        
        return window_handle;
    }
    
    int WinMain(HINSTANCE hInstance)
    {
        // Memory needs to be initialized before window. Discovered experimentally that the window will try to display as soon as it's created, before we create the buffer. Might be possible to re-order if initializing the window as non-visible.
        RootMemory* RootMemory = InitializeMemory();
        if (RootMemory == nullptr)
        {
            PostFatalMessage("Could not allocate memory");
            return 0;
        }
        HWND window_handle = InitializeWindow(hInstance);
        if (window_handle == nullptr)
        {
            PostFatalMessage("Could not create window");
            return 0;
        }
        
        Vector::int2 window_dimensions = GetWindowDimensions(window_handle);
        int width = window_dimensions.x;
        int height = window_dimensions.y;
        RootMemory->ScreenBuffer.Width = width;
        RootMemory->ScreenBuffer.Height = height;
        PopulateBitmapInfo(width, height);
        
        /* timeBeginPeriod() has a misleading name. It will set the Windows
         * Scheduler granulatiry (in milliseconds). This will determine how
         * frequenly the scheduler checks our resume condition once we call
         * Sleep().
        */
        MMRESULT mm_result = timeBeginPeriod(1);
        if (mm_result != TIMERR_NOERROR)
        {
            PostFatalMessage("Could not set scheduler granularity");
            return 0;
        }
        
        Clock::Initialize();
        InitialTime = Clock::GetTimeMilli();
        
        float target_framerate = 30.0; // hertz
        float target_frametime = 1000000.0 / target_framerate; // microseconds
        
        Engine::InitializeGame(RootMemory);
        
        Rendering::InitVulkan(hInstance, window_handle);
        
        // End initialization

        Fluff(RootMemory);

        // Start loop
        int loop_time_stamp = Clock::GetTimeMicro();
        int frame_time = 1; // 0 will cause a divid by zero error on first frame
        float work_load = 0;
        
        running = true;
        while (running)
        {
            ProcessMessages();
            ProcessMouse(window_handle, &RootMemory->ControlInput);
            
            RootMemory->DebugInfo.FrameRate = (int) (1000000.0 / frame_time);
            RootMemory->DebugInfo.WorkLoad = work_load;
            RootMemory->DebugInfo.Time = loop_time_stamp / 1000000.0;
            RootMemory->DebugInfo.CursorPosition = RootMemory->ControlInput.CursorPosition;
            
            Engine::GameMain(&RootMemory->ScreenBuffer, &RootMemory->ControlInput, &RootMemory->DebugInfo);

            int sleep_duration;
            
            // Not sure if it's neccesary to check sleep more than once.
            {
                frame_time = Clock::GetTimeMicro() - loop_time_stamp;
                sleep_duration = target_frametime - frame_time;
                if (sleep_duration > 0)
                {
                    Sleep(sleep_duration / 1000);
                }
                work_load = (target_frametime - sleep_duration) /  target_frametime;
                //Print("work_load = %f\n", work_load);
            }
            //while (sleep_duration > 0);
            //Print("Time elapsed = %u\n", frame_time);
            loop_time_stamp = Clock::GetTimeMicro();
            
            //RefreshScreen(window_handle, &RootMemory->ScreenBuffer);
            Rendering::DrawFrame();
        }
        
        return 0;
    }
}

int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    return WindowsOS::WinMain(hInstance);
}