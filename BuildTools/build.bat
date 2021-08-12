@echo off

if not exist BuildTools\config.py (
    copy BuildTools\config_template.py BuildTools\config.py 1>NUL
)

set my_batch=build\_build.bat
python BuildTools\prepare_build.py %my_batch%
%my_batch%