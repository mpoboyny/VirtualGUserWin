@echo off
echo.

set VGU_BIN_DIR=bin_trace_windows_x64_vs\
set VGU_EXE=virtualGUser-1.3.5.exe

set VGU_RUN=%~dp0..\bin\%VGU_BIN_DIR%%VGU_EXE%

call %VGU_RUN% %*

echo Done.
