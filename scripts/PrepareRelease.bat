@echo off
rem echo off
rem This gets the patch of this batch file.
rem Got it from http://stackoverflow.com/questions/3827567/how-to-get-the-path-of-the-batch-script-in-windows
SET THISPATH=%~dp0
rem This removes the '\' at the end
rem echo %mypath:~0,-1%

pushd %THISPATH%

rem Copy the interpreter we want to use to a temporary exe, so we can build the solution
rem This is necessary so that we are not locking the exe when the solution tries to write it.
copy %THISPATH%..\interpreters\squirrelsh\squirrelsh.exe %THISPATH%..\interpreters\squirrelsh\squirrelsh_tmp.exe

rem Execute
%THISPATH%..\interpreters\squirrelsh\squirrelsh_tmp.exe PrepareRelease.nut

popd


