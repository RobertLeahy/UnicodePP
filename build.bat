@echo off
robocopy ./build/windows ./ 2> nul > nul
make %*
