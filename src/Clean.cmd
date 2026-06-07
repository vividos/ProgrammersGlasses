@echo off
REM
REM Programmer's Glasses - a developer's file content viewer
REM Copyright (C) 2023-2026 Michael Fink
REM
REM Cleans project folders for archiving
REM

rmdir .vs /s /q
rmdir .sonarqube /s /q 2> nul
REM rmdir ProgrammersGlasses\.sonar-cache /s /q 2> nul
REM rmdir .sonar-cache /s /q 2> nul

del /S /Q *.user 2> nul
del /S /Q *.aps 2> nul
del /S /Q *.bml 2> nul

del ProgrammersGlasses\userinterface\res\Ribbon.h
del ProgrammersGlasses\userinterface\res\Ribbon.rc

rmdir ..\bin /s /q
rmdir ..\intermediate /s /q

pause
