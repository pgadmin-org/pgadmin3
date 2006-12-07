@echo off
REM #######################################################################
REM #
REM # pgAdmin III - PostgreSQL Tools
REM # Copyright (C) 2002 - 2006, The pgAdmin Development Team
REM # This software is released under the Artistic Licence
REM #
REM # stringextract.bat - extract strings from sources 
REM # and create gettext template file
REM #
REM #######################################################################

copy pgadmin3-release.pot pgadmin3.pot

xgettext -k_ -k__ -j -s -o pgadmin3.pot pgadmin/*.cpp
xgettext -k_ -k__ -j -s -o pgadmin3.pot pgadmin/agent/*.cpp
xgettext -k_ -k__ -j -s -o pgadmin3.pot pgadmin/base/*.cpp
xgettext -k_ -k__ -j -s -o pgadmin3.pot pgadmin/ctl/*.cpp
xgettext -k_ -k__ -j -s -o pgadmin3.pot pgadmin/db/*.cpp
xgettext -k_ -k__ -j -s -o pgadmin3.pot pgadmin/dlg/*.cpp
xgettext -k_ -k__ -j -s -o pgadmin3.pot pgadmin/frm/*.cpp
xgettext -k_ -k__ -j -s -o pgadmin3.pot pgadmin/main/*.cpp
xgettext -k_ -k__ -j -s -o pgadmin3.pot pgadmin/schema/*.cpp
xgettext -k_ -k__ -j -s -o pgadmin3.pot pgadmin/slony/*.cpp
xgettext -k_ -k__ -j -s -o pgadmin3.pot pgadmin/utils/*.cpp

xgettext -k_ -k__ -j -s -o pgadmin3.pot pgadmin/include/*.h
xgettext -k_ -k__ -j -s -o pgadmin3.pot pgadmin/include/base/*.h
xgettext -k_ -k__ -j -s -o pgadmin3.pot pgadmin/include/ctl/*.h
xgettext -k_ -k__ -j -s -o pgadmin3.pot pgadmin/include/nodes/*.h
xgettext -k_ -k__ -j -s -o pgadmin3.pot pgadmin/include/parser/*.h
xgettext -k_ -k__ -j -s -o pgadmin3.pot pgadmin/agent/include/*.h
xgettext -k_ -k__ -j -s -o pgadmin3.pot pgadmin/slony/include/*.h

xgettext -k_ -k__ -j -s -o pgadmin3.pot xtra/pgagent/*.cpp
xgettext -k_ -k__ -j -s -o pgadmin3.pot xtra/pgagent/include/*.h

wxrc -g pgadmin/ui/*.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -

