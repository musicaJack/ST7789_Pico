@echo off
echo Check Pico device...
set FOUND=0
for %%D in (D E F G H I J K L M N O P Q R S T U V W X Y Z) do (
  if exist %%D:\INFO_UF2.TXT (
    set PICO_DRIVE=%%D:
    set FOUND=1
  )
)

if %FOUND% EQU 0 (
  echo No Pico device found!
  echo Please press the BOOTSEL button and connect the Pico to the computer.
  pause
  exit /b 1
)

echo Pico device found: %PICO_DRIVE%
echo Deploying UF2 file...
copy /Y "build\pico_spi_lcd.uf2" %PICO_DRIVE%\
if %ERRORLEVEL% NEQ 0 (
  echo Copy failed!
  pause
  exit /b %ERRORLEVEL%
)

echo UF2 file deployed successfully to Pico!
timeout /t 3 