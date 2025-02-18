@echo off

:: save the current directory
set ORIGINAL_DIR=%cd%

:: build the project 
cmake --build build

:: cd to output dir 
cd .\build\Debug || (
    echo Directory .\build\Debug does not exist. Exiting.
    exit /b 1
)

:: run exe 
if exist main.exe (
    main.exe
) else (
    echo Executable game.exe not found in .\build\Debug.
    cd %ORIGINAL_DIR%
    exit /b 1
)

:: cd back to original directory 
cd %ORIGINAL_DIR%
