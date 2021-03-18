@echo off
set exe=build\main.exe
if not exist %exe% (
    echo Cannot find %exe%
) else (
    devenv %exe%
)