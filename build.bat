@echo off

if not exist build mkdir build

if exist build\bin rmdir /s /q build\bin
mkdir build\bin

if not exist build\AppData mkdir build\AppData

set build_output= -Febuild\bin\main.exe -Fobuild\bin\ -Fdbuild\bin\

:: -Zi => debug information (for VS)
set build_flags= -Zi

set build_sdk_include= -I src\
set build_sdk_include=%build_sdk_include% -I C:\GLM\glm\
set build_sdk_include=%build_sdk_include% -I C:\VulkanSDK\1.2.162.1\Include\vulkan

:: Libraries to include
set build_libs=
:: user32.lib provides multiple OS services from <window.h>
set build_libs=%build_libs% user32.lib
:: gdi32.lib provides StretchDIBits() from <windows.h>
set build_libs=%build_libs% gdi32.lib
:: winmm.lib provides timeBeginPeriod() from <windows.h>
set build_libs=%build_libs% winmm.lib
set build_libs=%build_libs% C:\VulkanSDK\1.2.162.1\Lib\vulkan-1.lib

:: Header files to include everywhere
set build_include=
set build_include=%build_include% -FICommon.h
set build_include=%build_include% -FIConfig.h
set build_include=%build_include% -FIStandard\Standard.h
set build_include=%build_include% -FIMemory.h
set build_include=%build_include% -FImalloc.h
set build_include=%build_include% -FImath.h
set build_include=%build_include% -FIWP\Print.h

:: source files to be compiled
set build_source=
set build_source=%build_source% src\WP\WinMain.cpp
set build_source=%build_source% src\WP\Clock.cpp
set build_source=%build_source% src\WP\Print.cpp
set build_source=%build_source% src\Engine\GameMain.cpp
set build_source=%build_source% src\Engine\OpenBitmap.cpp
set build_source=%build_source% src\Standard\Util.cpp
set build_source=%build_source% src\Engine\Rendering.cpp
set build_source=%build_source% src\Standard\Vector.cpp
set build_source=%build_source% src\Engine\AssetManager.cpp
set build_source=%build_source% src\Standard\String.cpp
set build_source=%build_source% src\WP\Render.cpp

:: cl @command_file.txt
cl%build_output%%build_flags%%build_include%%build_libs%%build_sdk_include%%build_source%