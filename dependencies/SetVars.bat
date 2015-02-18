@echo off

call:check_msbuild
if %ERRORLEVEL% NEQ 0 exit /b
call:check_svn
if %ERRORLEVEL% NEQ 0 exit /b
call:check_cmake
if %ERRORLEVEL% NEQ 0 exit /b
call:check_python
if %ERRORLEVEL% NEQ 0 exit /b
goto :EOF

rem *******************************
rem * MSBuild
rem *******************************
:check_msbuild
	msbuild /version > nul
	if %ERRORLEVEL% EQU 0 exit /b
	echo WARNING: MSBuild not found. Trying to initialize default VC environment.
	call "%VS120COMNTOOLS%..\..\VC\vcvarsall.bat" x86
	rem Try again
	msbuild /version > nul
	if %ERRORLEVEL% EQU 0 exit /b
	rem error
	echo ERROR: Visual studio environment variables not set, and failed to set default VS 2013 environment. Setup your own, and run this script again.
	exit /b
	
rem *******************************
rem * SVN
rem *******************************
:check_svn
	svn --version > nul
	if %ERRORLEVEL% EQU 0 exit /b
	echo ERROR: SVN not found.
	exit /b

rem *******************************
rem * Cmake
rem *******************************
:check_cmake
	cmake --version > nul
	if %ERRORLEVEL% EQU 0 exit /b
	echo ERROR: CMake not found.
	exit /b

rem *******************************
rem * Python
rem *******************************
:check_python
	python --version > nul
	if %ERRORLEVEL% EQU 0 exit /b
	echo ERROR: Python not found.
	exit /b
