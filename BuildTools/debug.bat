@echo off
set sln=build\main.sln
if not exist %sln% (
    echo Cannot find %sln%
) else (
    devenv %sln%
)