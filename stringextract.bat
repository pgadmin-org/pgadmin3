@echo off
REM #######################################################################
REM #
REM # pgAdmin III - PostgreSQL Tools
REM # Copyright (C) 2002 - 2005, The pgAdmin Development Team
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
xgettext -k_ -k__ -j -s -o pgadmin3.pot src/slony/*.cpp
xgettext -k_ -k__ -j -s -o pgadmin3.pot src/slony/include/*.h
xgettext -k_ -k__ -j -s -o pgadmin3.pot src/agent/*.cpp
xgettext -k_ -k__ -j -s -o pgadmin3.pot src/agent/include/*.h
wxrc -g src/ui/common/*.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -

