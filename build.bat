@echo off
robocopy ./build/windows ./ 2> nul > nul
..\MCPP\gcc48\bin\mingw32-make.exe %*