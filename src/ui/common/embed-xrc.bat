@echo off
REM #######################################################################
REM #
REM # pgAdmin III - PostgreSQL Tools
REM # Copyright (C) 2002 - 2005, The pgAdmin Development Team
REM # This software is released under the Artistic Licence
REM #
REM # embed-xrc.bat - convert xrc files to c++ files
REM #
REM #######################################################################

wxrc -c -o xrcDialogs.cpp *.xrc

