@echo off
cls
echo This file will upgrade your pgAdmin III installation.
echo.
echo You must have pgAdmin III v1.8.x installed 
echo from the official MSI installation to use this upgrade path.
echo.
echo If pgAdmin III or any of its components are in use
echo a reboot will be required once the upgrade is completed.
echo.
echo.
echo Press Ctrl-C to abort the upgrade or
pause

REM Parameters described:
REM  /i pgadmin3.msi           - pick MSI file to install. All properties
REM                              will be read from existing installation.
REM  REINSTALLMODE=vamus       - reinstall all files, regardless of version.
REM                              This makes sure documentation and other
REM                              non-versioned files are updated.
REM  REINSTALL=ALL             - Reinstall all features that were previously
REM                              installed with the new version.
msiexec /i pgadmin3.msi REINSTALLMODE=vamus REINSTALL=ALL /qr
