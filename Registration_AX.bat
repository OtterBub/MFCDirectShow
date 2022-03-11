@echo off

@echo ##
@echo ## Registered Filter AX File
@echo ## 20220310 Created by SungKyoung Park (VIDBOX INC.)
@echo ##
@echo ## --- Patch Note ---
@echo ## -- 20220310
@echo ## First Create

:-------------------------------------
>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"

if '%errorlevel%' NEQ '0' (
    echo ## Request Admin Permissions
    echo ##
    goto UACPrompt
) else ( goto gotAdmin )

:UACPrompt
    echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
    echo UAC.ShellExecute "%~s0", "", "", "runas", 1 >> "%temp%\getadmin.vbs"

    "%temp%\getadmin.vbs"
    if exist "%temp%\getadmin.vbs" ( del "%temp%\getadmin.vbs" )

    exit /B

:gotAdmin
    if exist "%temp%\getadmin.vbs" ( del "%temp%\getadmin.vbs" )
    pushd "%CD%"
    CD /D "%~dp0"
:---------------------------------
@echo on

@echo off
@cls
@echo ##
@echo ## Registered Filter AX
@echo ## 20220310 Created by Park SungKyoung (VIDBOX INC.)
@echo ## ================================================
@echo ##
@echo ## 1. Release
@echo ## 2. Debug
@echo ##
@echo ## ================================================
set /p choice=Please choose 1 OR 2 :


if "%choice%"=="1" ( goto RELEASE )
if "%choice%"=="2" ( goto DEBUG )

:RELEASE
REM Release
@echo ## "Release Run Registering Filter ax"
@echo on
for %%i in (%cd%\bin\Release\*.ax) do regsvr32.exe /s %%i
goto END

:DEBUG
REM Debug
@echo ## "Debug Run Registering Filter AX"
@echo on
for %%i in (%cd%\bin\Debug\*.ax) do regsvr32.exe /s %%i
goto END


:END
@echo off
@echo ##
@echo ##
@echo ## "Complete Registered Filter AX"
@echo ##
@echo ## "Plase Any Key ...."
@pause > nul