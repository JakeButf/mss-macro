@echo off
REM Set your Wii U IP address here or pass as environment variable
if not defined WIIU_IP set WIIU_IP=192.168.1.165

echo Building plugin...
docker run --rm -v %cd%:/project mss-macro make

if %errorlevel% neq 0 (
    echo Build failed!
    exit /b %errorlevel%
)

echo Sending to Wii U at %WIIU_IP%...
docker run --rm -v %cd%:/project -e WIILOAD=tcp:%WIIU_IP% vz /opt/devkitpro/tools/bin/wiiload /project/mss-macro.wps

if %errorlevel% neq 0 (
    echo Failed to send to Wii U!
    exit /b %errorlevel%
)

echo Done!







