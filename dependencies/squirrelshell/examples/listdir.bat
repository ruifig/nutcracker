@echo off

rem List directory contents.

set pth=.
if not "%1" == "" set pth=%1
for %%f in (%pth%\*) do echo %%f
