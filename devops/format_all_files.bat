@echo off
CD ..\sources
FOR /R %%f IN (*.cpp, *.hpp, *.h) DO (
    clang-format -i %%f
    echo %%f
)
PAUSE
