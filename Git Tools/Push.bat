@echo off
echo Running sync (pushing to GitHub)...

REM Change to the directory this script is in
cd /d "%~dp0.."


REM Ensure this is a git repo
if not exist ".git" (
    echo.
    echo Error: Not a git repository.
    exit /b
)

REM Prompt for a commit message
set /p commitmsg=Enter commit message (default: Auto push from Git Push script): 

REM Use default if empty
if "%commitmsg%"=="" set commitmsg=Auto push from Git Push script



echo Commit message: "%commitmsg%"


echo.
echo "=== Staging, committing, and pushing ==="
echo.

REM Add all changes, commit, and push
git add --all
git status
git commit -m "%commitmsg%"
git branch -M master
git push origin master

echo.
echo Push complete!
pause
