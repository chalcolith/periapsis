rem @echo off
rem usage: process_file.bat command dest_dir library_zipfile orig_dir renamed_dir [desired_dir desired_file]...

set command=%1
shift

set destdir=%1
shift

set zipfile=%1
shift

set origdir=%1
shift

set rendir=%1
shift


if "%command%"=="build" goto :build
if "%command%"=="clean" goto :clean
goto nocommand

:clean
echo clean > process_file.tmp
if exist "..\..\%rendir%" echo Deleting "..\..\%rendir%" ...
if exist "..\..\%rendir%" rmdir /q /s "..\..\%rendir%" >> process_file.tmp
if errorlevel 1 exit /b 1
if exist "..\..\%origdir%" echo Deleting "..\..\%origdir%" ...
if exist "..\..\%origdir%" rmdir /q /s "..\..\%origdir%" >> process_file.tmp
if errorlevel 1 exit /b 1

:cleanfile
set desireddir=%1
shift
set desiredfile=%1
shift

if "%desireddir%"=="" goto done
if exist %destdir%\%desiredfile% echo Deleting %desiredfile% ...
if exist %destdir%\%desiredfile% del /q %destdir%\%desiredfile% >> process_file.tmp
if errorlevel 1 exit /b 1

goto :cleanfile


:build
echo build > process_file.tmp

:processfile
set desireddir=%1
shift
set desiredfile=%1
shift

if "%desireddir%"=="" goto done
if exist "..\..\%rendir%\%desireddir%\%desiredfile%" goto copyfile

rem if exist "..\..\%origdir%" echo Deleting "..\..\%origdir%" ...
rem if exist "..\..\%origdir%" rmdir /q /s "..\..\%origdir%" >> process_file.tmp
rem if exist "..\..\%rendir%" echo Deleting "..\..\%rendir%" ...
rem if exist "..\..\%rendir%" rmdir /q /s "..\..\%rendir%" >> process_file.tmp
if not exist "..\..\%origdir%" echo Uncompressing "..\..\win32\%zipfile%" ...
if not exist "..\..\%origdir%" ..\..\win32\unzip -q "..\..\win32\%zipfile%" -d ..\.. >> process_file.tmp
if errorlevel 1 exit /b 1
if not exist "..\..\%rendir%" echo Moving "..\..\%origdir%" to "..\..\%rendir%" ...
if not exist "..\..\%rendir%" move "..\..\%origdir%" "..\..\%rendir%" >> process_file.tmp
if errorlevel 1 exit /b 1

:copyfile
if not exist %destdir% echo Creating %destdir% ...
if not exist %destdir% mkdir %destdir%  >> process_file.tmp
if errorlevel 1 exit /b 1
if not exist %destdir%\%desiredfile% echo Copying "..\..\%rendir%\%desireddir%\%desiredfile%" to %destdir% ...
if not exist %destdir%\%desiredfile% copy "..\..\%rendir%\%desireddir%\%desiredfile%" %destdir% >> process_file.tmp
if errorlevel 1 exit /b 1

goto processfile

:nocommand
echo "%command%" is not a valid command for process_file.bat
exit /b 1

:done
