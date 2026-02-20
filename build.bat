@echo off
REM MiniC 编译器构建和编译脚本 (Windows)

REM 编译翻译器
echo Building MiniC compiler...
gcc -o miniic.exe *.c

if %errorlevel% neq 0 (
    echo Build failed!
    exit /b 1
)

echo Build successful!
echo.

REM 检查是否提供了demo文件参数
if "%~1"=="" (
    echo Usage: build.bat ^<demo_file^>
    echo.
    echo Available demos:
    echo   demo\hello.c
    echo   demo\factorial.c
    echo   demo\fibonacci.c
    echo   demo\prime.c
    exit /b 1
)

set DEMO_FILE=%~1
for %%i in ("%DEMO_FILE%") do set BASENAME=%%~ni
set ASM_FILE=%BASENAME%.s
set OBJ_FILE=%BASENAME%.o
set EXE_FILE=%BASENAME%.exe

echo Compiling %DEMO_FILE%...
miniic.exe %DEMO_FILE% > %ASM_FILE%

if %errorlevel% neq 0 (
    echo Compilation failed!
    exit /b 1
)

echo Assembly to EXE not supported on Windows ^(^NASM not available^)
echo You can copy the assembly file to a Linux system to complete compilation
echo or use WSL to run: nasm -f elf32 %ASM_FILE% -o %OBJ_FILE%
echo then: gcc -m32 %OBJ_FILE% -o %EXE_FILE%
