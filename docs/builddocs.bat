@echo off

REM Compile the HTML Help
"%ProgramFiles%\HTML Help Workshop\hhc.exe" ./en_US/pgadmin3.hhp

REM Regenerate the cache file used by the wx viewer.
"%WXWIN%\utils\hhp2cached\vc_mswu\hhp2cached.exe" ./en_US/pgadmin3.hhp

EXIT 0