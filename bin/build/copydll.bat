@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ===== DLL 배포 스크립트 시작 =====

:: ================================
:: 1. 빌드 구성 선택
:: ================================
echo.
echo [1] 빌드 구성 선택
echo 1. Debug
echo 2. Release
set /p CFG=번호 입력:

if "%CFG%"=="1" set CONFIG=Debug
if "%CFG%"=="2" set CONFIG=Release

if not defined CONFIG (
    echo 잘못된 입력
    pause
    exit /b
)

:: ================================
:: 2. Toolset 선택
:: ================================
echo.
echo [2] Toolset 선택
echo 1. v100 (VS2010)
echo 2. v140 (VS2015 이상)
set /p TOOL=번호 입력:

if "%TOOL%"=="1" set BOOST_FOLDER=lib32-msvc-10.0
if "%TOOL%"=="2" set BOOST_FOLDER=lib32-msvc-14.0

if not defined BOOST_FOLDER (
    echo 잘못된 입력
    pause
    exit /b
)

:: ================================
:: 3. 대상 EXE 폴더 입력
:: ================================
echo.
set /p TARGET=EXE 실행 파일이 있는 폴더 경로 입력:

if not exist "%TARGET%" (
    echo 대상 폴더가 존재하지 않음
    pause
    exit /b
)

:: ================================
:: 기준 경로 (native)
:: ================================
set BASE=%~dp0

:: ================================
:: 4. DVH DLL 복사 (Win32 고정)
:: ================================
echo.
echo [DVH] 복사 중...

set DVH_DLL=%BASE%bin\Win32\%CONFIG%

if exist "%DVH_DLL%" (
    copy "%DVH_DLL%\*.dll" "%TARGET%" /Y
) else (
    echo 경로 없음: %DVH_DLL%
)

:: ================================
:: 5. Boost DLL 복사
:: ================================
echo.
echo [Boost] 복사 중...

set BOOST_DLL=%BASE%ThirdParty\boost_1.65_1\%BOOST_FOLDER%

if exist "%BOOST_DLL%" (
    copy "%BOOST_DLL%\*.dll" "%TARGET%" /Y
) else (
    echo 경로 없음: %BOOST_DLL%
)

:: ================================
:: 6. SQLite DLL 복사
:: ================================
echo.
echo [SQLite] 복사 중...

for %%i in ("%BASE%..\..\..\..") do set ROOT=%%~fi

set SQLITE_DLL=%BASE%ThirdParty\sqlite3\lib

if exist "%SQLITE_DLL%" (
    copy "%SQLITE_DLL%\*.dll" "%TARGET%" /Y
) else (
    echo 경로 없음: %SQLITE_DLL%
)

echo.
echo ===== 완료 =====
pause