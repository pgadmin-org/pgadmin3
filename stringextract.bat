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

xgettext -k_ -k__ -j -s -o pgadmin3.pot src/*.cpp
xgettext -k_ -k__ -j -s -o pgadmin3.pot src/include/*.h
xgettext -k_ -k__ -j -s -o pgadmin3.pot src/db/*.cpp
xgettext -k_ -k__ -j -s -o pgadmin3.pot src/schema/*.cpp
xgettext -k_ -k__ -j -s -o pgadmin3.pot src/ui/*.cpp
xgettext -k_ -k__ -j -s -o pgadmin3.pot src/utils/*.cpp
wxrc -g src/ui/common/dlgAggregate.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/dlgCast.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/dlgCheck.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/dlgColumn.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/dlgConversion.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/dlgDatabase.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/dlgDomain.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/dlgForeignKey.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/dlgFunction.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/dlgGroup.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/dlgIndex.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/dlgIndexConstraint.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/dlgLanguage.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/dlgOperator.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/dlgRule.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/dlgSchema.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/dlgSequence.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/dlgTable.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/dlgTrigger.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/dlgType.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/dlgUser.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/dlgView.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/frmAddTableView.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/frmConnect.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/frmOptions.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/frmPassword.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/frmQBJoin.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/frmServer.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/frmStatus.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/frmVacuum.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -
wxrc -g src/ui/common/frmExport.xrc | xgettext -k_ -k__ -L C -j -s -o pgadmin3.pot -

