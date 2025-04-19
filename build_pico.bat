@echo off
echo Strart build Pico project...
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
if %ERRORLEVEL% NEQ 0 (
  echo build failed!
  pause
  exit /b %ERRORLEVEL%
)
echo build success!
echo UF2 file generated in build directory
pause 