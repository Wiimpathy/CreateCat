@echo OFF
setlocal EnableDelayedExpansion

set arg1=%1
REM set OPTION1=-debug
REM set OPTION2=-fuzzy
REM set OPTION3=-mobysearch
REM set OPTION4=-skiphidden


if "%~1" == "" goto TESTFOLDER


: TESTFOLDER
FOR %%i IN (%arg1%) DO IF EXIST %%~si\NUL GOTO FOLDER


:FILE
setlocal DISABLEDELAYEDEXPANSION
set DBFILE=%1
createcat.exe %DBFILE% %OPTION1% %OPTION2% %OPTION3% %OPTION4%
endlocal
GOTO QUIT


:FOLDER
setlocal disabledelayedexpansion
 for /f "delims=: tokens=1*" %%A in ('dir /b /s /a-d %arg1%^|findstr /n "^"') do (
	createcat.exe "%%B" %OPTION1% %OPTION2% %OPTION3% %OPTION4%
 )
setlocal EnableDelayedExpansion


GOTO QUIT

:QUIT
pause
