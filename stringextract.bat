@echo off
REM #######################################################################
REM #
REM # pgAdmin III - PostgreSQL Tools
REM # Copyright (C) 2002 - 2003, The pgAdmin Development Team
REM # This software is released under the Artistic Licence
REM #
REM # stringextract.bat - extract strings from sources 
REM # and create gettext template file
REM #
REM #######################################################################

copy pgadmin3-release.pot pgadmin3.pot
xgettext -k_ -k__ -j -s -o pgadmin3.pot src/*.cpp
xgettext -k_ -k__ -j -s -o pgadmin3.pot src/include/*.h
xgettext -k_ -k__ -j -s -o pgadmin3.pot src/db/*.cpp
xgettext -k_ -k__ -j -s -o pgadmin3.pot src/schema/*.cpp
xgettext -k_ -k__ -j -s -o pgadmin3.pot src/ui/*.cpp
xgettext -k_ -k__ -j -s -o pgadmin3.pot src/utils/*.cpp
FOR %%f IN (src/ui/common/*.xrc) DO wxrc -g src/ui/common/%%f | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -

