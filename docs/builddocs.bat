@echo off

REM Compile the HTML Help
"%ProgramFiles%\HTML Help Workshop\hhc.exe" ./cs_CZ/pgadmin3.hhp
"%ProgramFiles%\HTML Help Workshop\hhc.exe" ./en_US/pgadmin3.hhp
"%ProgramFiles%\HTML Help Workshop\hhc.exe" ./fr_FR/pgadmin3.hhp

REM Regenerate the cache file used by the wx viewer.
IF NOT EXIST %WXWIN%\utils\hhp2cached\vc_mswu\hhp2cached.exe GOTO NO_HHP2CACHED

"%WXWIN%\utils\hhp2cached\vc_mswu\hhp2cached.exe" ./cs_CZ/pgadmin3.hhp
"%WXWIN%\utils\hhp2cached\vc_mswu\hhp2cached.exe" ./en_US/pgadmin3.hhp
"%WXWIN%\utils\hhp2cached\vc_mswu\hhp2cached.exe" ./fr_FR/pgadmin3.hhp

EXIT 0

:NO_HHP2CACHED
ECHO Could not find %WXWIN%\utils\hhp2cached\vc_mswu\hhp2cached.exe!!
EXIT 1
