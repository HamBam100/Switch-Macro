@echo off
echo Running sync (pulling from GitHub)...

cd /d "%~dp0.."

REM Ensure this is a git repo
if not exist ".git" (
    echo.
    echo Error: Not a git repository.
    pause
    exit /b
)

git fetch origin
git pull origin master

echo.
echo Pull complete!
pause
