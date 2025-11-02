@echo off
echo Running GitHub Setup...

cd /d "%~dp0.."


git init
git config --global user.email "hambam100@gmail.com"
git config --global user.name "HamBam100"



REM Prompt for a commit message
set /p repo=Enter commit message (default: https://github.com/HamBam100/AGAM.git): 

REM If empty, use default
if "%repo%"=="" set commitmsg=https://github.com/HamBam100/AGAM.git




echo Pulling repo

git remote add origin "%repo%"
git pull origin master

echo Setup complete!
pause