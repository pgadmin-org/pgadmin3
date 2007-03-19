@echo off

REM
REM Setup the following values as required for the installation
REM

SET APPVENDOR=The pgAdmin Development Team
SET APPNAME=pgAdmin III
SET APPKEYWORDS=PostgreSQL, pgAdmin
SET APPCOMMENTS=PostgreSQL Tools
SET APPDESCRIPTION=Management and administration tools for the PostgreSQL DBMS

SET BUILDTREE="../.."

SET BRANDED=0
SET BRANDINGDIR="../../branding"

REM
REM END OF CONFIG SECTION
REM

if NOT "%1"=="" GOTO REGEN_GUIDS

echo Invalid command line options.
echo Usage: "Make.bat <Major.Minor version number>"
echo        "Make.bat REGENGUIDS"
echo.
GOTO EXIT



:REGEN_GUIDS

if NOT "%1"=="REGENGUIDS" GOTO BUILD_PACKAGE

echo.

echo Regenerating GUIDs in src/pgadmin3.wxs...
perl utils\regenguids.pl src/pgadmin3.wxs
move src\pgadmin3.wxs.out src\pgadmin3.wxs

echo Regenerating GUIDs in src/i18ndata.wxs...
perl utils\regenguids.pl ./src/i18ndata.wxs
move src\i18ndata.wxs.out src\i18ndata.wxs

echo.
echo Done!

GOTO EXIT



:BUILD_PACKAGE

echo.
echo Building %APPNAME% Installer...

candle -nologo -dBUILDTREE="%BUILDTREE%" -dBRANDED=%BRANDED% -dBRANDINGDIR="%BRANDINGDIR%" -dAPPVENDOR="%APPVENDOR%" -dAPPNAME="%APPNAME%" -dAPPKEYWORDS="%APPKEYWORDS%" -dAPPCOMMENTS="%APPCOMMENTS%" -dAPPDESCRIPTION="%APPDESCRIPTION%" -dAPPVERSION="%1" -dSYSTEM32DIR="%SystemRoot%\System32" -dPFILESDIR="%ProgramFiles%" src/pgadmin3.wxs
IF ERRORLEVEL 1 GOTO ERR_HANDLER

light -nologo -ext WixUIExtension -cultures:en-us pgadmin3.wixobj
IF ERRORLEVEL 1 GOTO ERR_HANDLER

echo.
echo Done!
GOTO EXIT

:ERR_HANDLER
echo.
echo Aborting build!
GOTO EXIT



:EXIT