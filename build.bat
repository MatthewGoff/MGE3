@echo off

set build_dir=build
rmdir /s /q %build_dir%
mkdir %build_dir%

pushd %build_dir%
cl @..\command_file.txt
popd