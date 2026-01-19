@echo off
setlocal enabledelayedexpansion

echo ==================================================
echo Project Structure Creator
echo ==================================================
echo.

REM Ask for project name
set /p PROJECT_NAME="What do you want to name your new project? "
if "%PROJECT_NAME%"=="" (
    echo Error: Project name cannot be empty!
    pause
    exit /b 1
)

REM Ask about Arduino
set /p ARDUINO_INPUT="Will you be using an Arduino? (yes/no): "
set ARDUINO_INPUT=%ARDUINO_INPUT:~0,1%
if /i "%ARDUINO_INPUT%"=="y" (
    set USE_ARDUINO=1
) else (
    set USE_ARDUINO=0
)

echo.
echo Creating project structure for '%PROJECT_NAME%'...

REM Create main project directory
if not exist "%PROJECT_NAME%" mkdir "%PROJECT_NAME%"
cd "%PROJECT_NAME%"

REM Create README.md
echo # %PROJECT_NAME% > README.md
echo. >> README.md
echo ## Overview >> README.md
echo Brief description of the project and the problem it addresses. >> README.md
echo. >> README.md
echo ## Objectives >> README.md
echo - What skills you aimed to develop >> README.md
echo - What the project was intended to accomplish >> README.md
echo. >> README.md
echo ## Requirements ^& Constraints >> README.md
echo - Functional requirements the design must meet >> README.md
echo - Physical, material, cost, or manufacturing constraints >> README.md
echo - Safety, size, or performance limitations >> README.md
echo. >> README.md
echo ## Design Summary >> README.md
echo High-level explanation of how the system works. >> README.md
echo Mention key components and design approach. >> README.md
echo. >> README.md
echo ## Tools ^& Technologies >> README.md
echo - CAD: >> README.md
echo - Electronics ^(if any^): >> README.md
echo - Manufacturing: >> README.md
echo - Software: >> README.md
echo. >> README.md
echo ## Testing ^& Results >> README.md
echo How the design was tested and the outcome. >> README.md
echo Mention key observations or performance. >> README.md
echo. >> README.md
echo ## Challenges ^& Lessons Learned >> README.md
echo Main issues encountered and what you learned from solving them. >> README.md
echo. >> README.md
echo ## Future Improvements >> README.md
echo Ideas for improvements or next iterations. >> README.md

REM Create CAD directory structure
mkdir CAD 2>nul
mkdir CAD\fusion360_files 2>nul
mkdir CAD\exported_STL 2>nul

REM Create Arduino directory if needed
if "%USE_ARDUINO%"=="1" (
    mkdir Arduino 2>nul
    (
        echo // Arduino code for %PROJECT_NAME%
        echo.
        echo void setup^(^) {
        echo   // Initialize your code here
        echo }
        echo.
        echo void loop^(^) {
        echo   // Main code loop
        echo }
    ) > Arduino\code.ino
    
    REM Create Electronics directory (only if using Arduino)
    mkdir Electronics 2>nul
    type nul > Electronics\wiring_diagram.png
)

REM Create Images directory
mkdir Images 2>nul
type nul > Images\prototype_v1.jpg
type nul > Images\final_build.jpg

REM Create Notes directory
mkdir Notes 2>nul
(
    echo # Design Log – %PROJECT_NAME%
    echo.
    echo ## Date: YYYY-MM-DD
    echo - **Work:** What you did
    echo - **Issues:** What didn't work or needs improvement
    echo - **Notes:** Engineering observations
    echo - **Next:** Next steps
) > Notes\design_log.md

cd ..

echo.
echo Project structure created successfully!
echo Location: %CD%\%PROJECT_NAME%
echo.
if "%USE_ARDUINO%"=="1" (
    echo Created folders:
    echo   - CAD\
    echo   - Arduino\
    echo   - Electronics\
    echo   - Images\
    echo   - Notes\
) else (
    echo Created folders:
    echo   - CAD\
    echo   - Images\
    echo   - Notes\
)
echo.
pause
