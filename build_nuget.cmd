@echo off
REM ============================================================
REM build_nuget.cmd - VisionMotionFramework NuGet Package Builder
REM ============================================================
REM
REM Usage:
REM   build_nuget              - Debug|Win32
REM   build_nuget Release      - Release|Win32
REM   build_nuget Debug x64    - Debug|x64
REM   build_nuget Release x64  - Release|x64
REM
REM Prerequisite: Build VisionMotionFramework.sln first.
REM   .lib files must be in bin\Platform\Config\
REM ============================================================

setlocal

set VMF_ROOT=%~dp0
set DEPLOY=%VMF_ROOT%NuGetDeploy

set CONFIG=Debug
set PLATFORM=Win32

if not "%1"=="" set CONFIG=%1
if not "%2"=="" set PLATFORM=%2

echo.
echo ============================================================
echo VisionMotionFramework NuGet Package Builder
echo   Config : %CONFIG%
echo   Platform: %PLATFORM%
echo ============================================================
echo.

REM ---- 1. Copy VMF Core headers ----
echo [1/5] Copying VMF Core headers...
if not exist "%DEPLOY%\build\native\include" mkdir "%DEPLOY%\build\native\include"
xcopy /E /Y /Q "%VMF_ROOT%VisionMotionFramework\include\*.h" "%DEPLOY%\build\native\include\" >nul 2>&1
echo   Done: %DEPLOY%\build\native\include\

REM ---- 2. Copy VisionComm headers ----
echo [2/5] Copying VisionComm headers...
if not exist "%DEPLOY%\build\native\include\VC" mkdir "%DEPLOY%\build\native\include\VC"
xcopy /E /Y /Q "%VMF_ROOT%VisionComm\include\*.h" "%DEPLOY%\build\native\include\VC\" >nul 2>&1
echo   Done: %DEPLOY%\build\native\include\VC\

REM ---- 3. Copy static libraries ----
echo [3/5] Copying static libraries...
if not exist "%DEPLOY%\lib\native\v142\%PLATFORM%" mkdir "%DEPLOY%\lib\native\v142\%PLATFORM%"

set BIN_DIR=%VMF_ROOT%bin\%PLATFORM%\%CONFIG%

if exist "%BIN_DIR%\VisionMotionFramework.lib" (
    copy /Y "%BIN_DIR%\VisionMotionFramework.lib" "%DEPLOY%\lib\native\v142\%PLATFORM%\VMF.Core.lib" >nul
    echo   VMF.Core.lib -^> %DEPLOY%\lib\native\v142\%PLATFORM%\VMF.Core.lib
) else (
    echo   [WARN] VisionMotionFramework.lib not found at %BIN_DIR%
)

if exist "%BIN_DIR%\VisionComm.lib" (
    copy /Y "%BIN_DIR%\VisionComm.lib" "%DEPLOY%\lib\native\v142\%PLATFORM%\VisionComm.lib" >nul
    echo   VisionComm.lib -^> %DEPLOY%\lib\native\v142\%PLATFORM%\VisionComm.lib
) else (
    echo   [WARN] VisionComm.lib not found at %BIN_DIR%
)

REM ---- 4. Verify .nuspec ----
echo [4/5] Verifying .nuspec file...
if exist "%DEPLOY%\VisionMotionFramework.Core.v142.nuspec" (
    echo   Found VisionMotionFramework.Core.v142.nuspec
) else (
    echo   [ERROR] .nuspec not found at %DEPLOY%
    goto ERROR
)

REM ---- 5. Pack NuGet ----
echo [5/5] Packing NuGet package...
cd /d "%DEPLOY%"
"%VMF_ROOT%nuget.exe" pack VisionMotionFramework.Core.v142.nuspec
if errorlevel 1 goto ERROR

echo.
echo ============================================================
echo NuGet package created successfully!
echo   Package: %DEPLOY%\VisionMotionFramework.Core.v142.*.nupkg
echo ============================================================
goto END

:ERROR
echo.
echo [ERROR] Failed to create NuGet package.
pause
exit /b 1

:END
endlocal
