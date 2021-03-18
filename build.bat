@echo off

set build_dir=build
rmdir /s /q %build_dir%
mkdir %build_dir%

pushd %build_dir%
cl -Zi ..\src\main.cpp user32.lib gdi32.lib
popd