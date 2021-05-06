@echo off

::if exist build\main.exe del build\main.exe :: Linker will reuse some of .exe
if not exist build mkdir build
if not exist build\AppData mkdir build\AppData
if not exist build\obj mkdir build\obj

python BuildTools\compile_command.py
cl @BuildTools\compile_command.txt

python BuildTools\link_command.py
link @BuildTools\link_command.txt