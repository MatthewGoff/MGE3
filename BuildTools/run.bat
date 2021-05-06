@echo off
set exe=build\bin\main.exe
if not exist %exe% (
    echo Cannot find %exe%
) else (
    start %exe%
)