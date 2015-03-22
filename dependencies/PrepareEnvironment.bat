@echo off
call SetVars.bat
if %ERRORLEVEL% NEQ 0 exit /b

rem wxWidgets
call:wxwidgets_build
if %ERRORLEVEL% NEQ 0 exit /b

echo DONE
goto :EOF

rem ****************************************
rem * wxWidgets
rem ****************************************

:wxwidgets_build
	copy wxWidgets\include\wx\msw\setup0.h wxWidgets\include\wx\msw\setup.h
	msbuild wxWidgets\build\msw\wx_vc12.sln /t:Build /p:Configuration=Debug;Platform=Win32 /verbosity:minimal /maxcpucount /nr:false
	if %ERRORLEVEL% NEQ 0 exit /b
	msbuild wxWidgets\build\msw\wx_vc12.sln /t:Build /p:Configuration=Release;Platform=Win32 /verbosity:minimal /maxcpucount /nr:false
	if %ERRORLEVEL% NEQ 0 exit /b
	msbuild wxWidgets\build\msw\wx_vc12.sln /t:Build /p:Configuration=Debug;Platform=x64 /verbosity:minimal /maxcpucount /nr:false
	if %ERRORLEVEL% NEQ 0 exit /b
	msbuild wxWidgets\build\msw\wx_vc12.sln /t:Build /p:Configuration=Release;Platform=x64 /verbosity:minimal /maxcpucount /nr:false
	if %ERRORLEVEL% NEQ 0 exit /b
	exit /b
