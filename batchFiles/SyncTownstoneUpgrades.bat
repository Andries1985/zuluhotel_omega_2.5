@echo off
setlocal

pushd "%~dp0.."

set "PYTHON_EXE=.venv\Scripts\python.exe"
if not exist "%PYTHON_EXE%" set "PYTHON_EXE=python"

set "DIRECTION=auto"
if /I "%~1"=="cfg-to-xlsx" set "DIRECTION=cfg-to-xlsx"
if /I "%~1"=="xlsx-to-cfg" set "DIRECTION=xlsx-to-cfg"
if /I "%~1"=="auto" set "DIRECTION=auto"

echo Running townstone upgrades sync (%DIRECTION%)...
"%PYTHON_EXE%" pythonscripts\_sync_townstone_upgrades.py --direction %DIRECTION%
set "EXIT_CODE=%ERRORLEVEL%"

if not "%EXIT_CODE%"=="0" (
    echo.
    echo Sync failed with exit code %EXIT_CODE%.
) else (
    echo.
    echo Sync completed successfully.
)

popd
endlocal & exit /b %EXIT_CODE%
