@echo off
REM Run the Python formatting script for the specified directory and extensions
python format.py ../include/ccmath --extensions .h,.hpp

REM Check the exit code to display an appropriate message
if %errorlevel% neq 0 (
    echo Error: Failed to run the format script.
    exit /b %errorlevel%
) else (
    echo Format script ran successfully.
)
