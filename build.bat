@echo off

if not exist build mkdir build
if not exist build\data mkdir build\data

if exist build\bin rmdir /s /q build\bin
mkdir build\bin

cl @command_file.txt