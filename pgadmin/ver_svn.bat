@echo off

set V=
set OV=

REM *****************************************
REM Get the SVN revision
REM *****************************************

if not exist .svn goto git

if exist include\svnversion.h for /f "tokens=3" %%f in (include\svnversion.h) do set OV=%%f

for /f %%f in ('svnversion .') do set V=%%f

if "%V%"==%OV% goto same

echo Generating svnversion.h for revision %V%.
echo #define VERSION_SVN "%V%" > include\svnversion.h
goto :eof

REM *****************************************
REM Get the GIT revision
REM *****************************************

:git

if not exist ..\.git goto nosvnorgit

if exist include\svnversion.h for /f "tokens=3" %%f in (include\svnversion.h) do set OV=%%f

for /f %%f in ('git describe --always') do set V=%%f

if "%V%"==%OV% goto same

echo Generating svnversion.h for revision %V%.
echo #define VERSION_SVN "%V%" > include\svnversion.h
goto :eof

REM *****************************************
REM No change...
REM *****************************************

:same
echo Not generating svnversion.h, not changed.
goto :eof

REM *****************************************
REM No source control
REM *****************************************

:nosvnorgit
echo Not generating svnversion.h, not working in a svn or git checkout.
if not exist include\svnversion.h echo #define VERSION_SVN "unknown" > include\svnversion.h
goto :eof


