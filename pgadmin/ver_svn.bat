@echo off

set V=
set OV=

if not exist .svn goto nosvn

if exist include\svnversion.h for /f "tokens=3" %%f in (include\svnversion.h) do set OV=%%f

for /f %%f in ('svnversion .') do set V=%%f

if "%V%"==%OV% goto same

echo Generating svnversion.h for revision %V%.
echo #define VERSION_SVN "%V%" > include\svnversion.h
goto :eof

:same
echo Not generating svnversion.h, not changed.
goto :eof

:nosvn
echo Not generating svnversion.h, not working in a svn checkout.
goto :eof

