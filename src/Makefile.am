# pgAdmin III - PostgreSQL Tools
# Copyright (C) 2002 - 2003, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# Makefile - Makefile for *nix systems
bin_PROGRAMS = pgadmin3

#  db/keywords.c  needs to be added

pgadmin3_SOURCES = pgAdmin3.cpp \
db/pgConn.cpp db/pgSet.cpp \
schema/pgAggregate.cpp schema/pgCast.cpp \
schema/pgCheck.cpp schema/pgCollection.cpp \
schema/pgColumn.cpp schema/pgConstraints.cpp \
schema/pgConversion.cpp schema/pgDatabase.cpp \
schema/pgDomain.cpp schema/pgForeignKey.cpp \
schema/pgFunction.cpp schema/pgGroup.cpp \
schema/pgIndex.cpp schema/pgIndexConstraint.cpp \
schema/pgLanguage.cpp schema/pgObject.cpp \
schema/pgOperator.cpp schema/pgOperatorClass.cpp \
schema/pgRule.cpp schema/pgSchema.cpp \
schema/pgSequence.cpp schema/pgServer.cpp \
schema/pgTable.cpp schema/pgTrigger.cpp \
schema/pgType.cpp schema/pgUser.cpp schema/pgView.cpp \
schema/pgDatatype.cpp \
ui/ctlSQLBox.cpp ui/ctlSQLResult.cpp ui/frmExport.cpp \
ui/dlgAddTableView.cpp ui/events.cpp ui/frmAbout.cpp \
ui/frmChildTableViewFrame.cpp ui/frmConnect.cpp \
ui/frmMain.cpp ui/frmOptions.cpp ui/frmPassword.cpp \
ui/frmQuery.cpp ui/frmQueryBuilder.cpp ui/frmHelp.cpp \
ui/frmQBJoin.cpp ui/frmSplash.cpp ui/frmMaintenance.cpp \
ui/frmEditGrid.cpp ui/dlgProperty.cpp ui/dlgUser.cpp \
ui/dlgGroup.cpp ui/dlgDatabase.cpp ui/dlgLanguage.cpp \
ui/dlgSchema.cpp ui/dlgDomain.cpp ui/dlgTable.cpp \
ui/dlgColumn.cpp ui/dlgIndex.cpp ui/dlgFunction.cpp \
ui/dlgView.cpp ui/dlgRule.cpp ui/dlgOperator.cpp \
ui/dlgAggregate.cpp ui/dlgCast.cpp ui/dlgConversion.cpp \
ui/dlgIndexConstraint.cpp ui/dlgForeignKey.cpp \
ui/dlgSequence.cpp ui/dlgTrigger.cpp ui/dlgType.cpp \
ui/dlgCheck.cpp ui/frmStatus.cpp utils/misc.cpp \
utils/sysLogger.cpp utils/sysSettings.cpp \
ui/common/xrcDialogs.cpp

noinst_HEADERS = include/copyright.h include/version.h include/ctlSQLBox.h \
include/ctlSQLResult.h include/dlgAddTableView.h \
include/parser/parse.h include/parser/keywords.h include/nodes/parsenodes.h \
include/postgres.h include/pgDefs.h include/pgDatatype.h \
include/dlgCheck.h include/dlgColumn.h \
include/dlgDatabase.h include/dlgDomain.h include/frmExport.h \
include/dlgForeignKey.h include/dlgFunction.h include/dlgGroup.h \
include/dlgIndexConstraint.h include/dlgIndex.h \
include/dlgLanguage.h include/dlgProperty.h include/dlgType.h \
include/dlgView.h include/dlgRule.h include/dlgOperator.h \
include/dlgAggregate.h include/dlgCast.h include/dlgConversion.h \
include/dlgSequence.h include/dlgSchema.h include/dlgTable.h \
include/dlgOperator.h include/dlgView.h include/dlgRule.h \
include/dlgTrigger.h include/dlgUser.h include/frmAbout.h \
include/frmChildTableViewFrame.h include/frmConnect.h \
include/frmEditGrid.h include/frmMain.h \
include/frmOptions.h include/frmPassword.h \
include/frmQBJoin.h include/frmQueryBuilder.h include/frmHelp.h \
include/frmQuery.h include/frmSplash.h \
include/frmStatus.h include/frmMaintenance.h \
include/misc.h include/pgAdmin3.h include/pgAggregate.h \
include/pgCast.h include/pgCheck.h include/pgCollection.h \
include/pgColumn.h include/pgConn.h \
include/pgConstraints.h include/pgConversion.h \
include/pgDatabase.h include/pgDomain.h \
include/pgForeignKey.h include/pgFunction.h \
include/pgGroup.h include/pgIndexConstraint.h \
include/pgIndex.h include/pgLanguage.h include/pgObject.h \
include/pgOperatorClass.h include/pgOperator.h \
include/pgRule.h include/pgSchema.h include/pgSequence.h \
include/pgServer.h include/pgSet.h include/pgTable.h \
include/pgTrigger.h include/pgType.h include/pgUser.h \
include/pgView.h include/sysLogger.h \
include/sysSettings.h include/wxgridsel.h include/menu.h
  
EXTRA_DIST = pgAdmin3.dsp pgAdmin3.dsw \
include/images/aggregate.xpm \
include/images/back.xpm \
include/images/help.xpm include/images/help2.xpm \
include/images/baddatabase.xpm include/images/cast.xpm \
include/images/check.xpm include/images/clip_copy.xpm \
include/images/clip_cut.xpm include/images/clip_paste.xpm \
include/images/closeddatabase.xpm \
include/images/closedown.xpm include/images/closeup.xpm \
include/images/column.xpm include/images/connect.xpm \
include/images/constraints.xpm \
include/images/conversion.xpm include/images/create.xpm \
include/images/database.xpm include/images/delete.xpm \
include/images/dnd_copy.xpm \
include/images/dnd_move.xpm include/images/dnd_none.xpm \
include/images/domain.xpm include/images/drop.xpm \
include/images/edit_clear.xpm include/images/edit_find.xpm \
include/images/edit_redo.xpm include/images/edit_undo.xpm \
include/images/elephant48.xpm \
include/images/encoding.xpm include/images/event.xpm \
include/images/file_open.xpm include/images/file_save.xpm \
include/images/foreignkey.xpm include/images/forward.xpm \
include/images/function.xpm \
include/images/group.xpm \
include/images/index.xpm \
include/images/key.xpm include/images/language.xpm \
include/images/namespace.xpm \
include/images/operatorclass.xpm \
include/images/operator.xpm include/images/pgAdmin3.ico \
include/images/pgAdmin3.xpm include/images/primarykey.xpm \
include/images/properties.xpm include/images/property.xpm \
include/images/public.xpm include/images/query_cancel.xpm \
include/images/query_execute.xpm \
include/images/query_explain.xpm include/images/readdata.xpm \
include/images/refresh.xpm include/images/relationship.xpm \
include/images/reload.xpm include/images/rule.xpm \
include/images/sequence.xpm include/images/serverbad.xpm \
include/images/servers.xpm include/images/server.xpm \
include/images/splash.xpm include/images/sql.xpm \
include/images/statistics.xpm include/images/storage.xpm \
include/images/storedata.xpm include/images/table.xpm \
include/images/triggerfunction.xpm \
include/images/trigger.xpm include/images/type.xpm \
include/images/unique.xpm include/images/unknown.xpm \
include/images/user.xpm include/images/vacuum.xpm \
include/images/viewdata.xpm include/images/view.xpm \
ui/af_ZA/pgadmin3.po \
ui/ar/pgadmin3.po \
ui/ar_SA/pgadmin3.po \
ui/cs_CZ/pgadmin3.po \
ui/da_DK/pgadmin3.po \
ui/de_DE/pgadmin3.po \
ui/es_ES/pgadmin3.po \
ui/fa_IR/pgadmin3.po \
ui/fr_FR/pgadmin3.po \
ui/hr_HR/pgadmin3.po \
ui/hu_HU/pgadmin3.po \
ui/id_ID/pgadmin3.po \
ui/is_IS/pgadmin3.po \
ui/it_IT/pgadmin3.po \
ui/ja_JP/pgadmin3.po \
ui/nb_NO/pgadmin3.po \
ui/pl_PL/pgadmin3.po \
ui/pt_BR/pgadmin3.po \
ui/ro_RO/pgadmin3.po \
ui/ru_RU/pgadmin3.po \
ui/sd_IN/pgadmin3.po \
ui/sv_SE/pgadmin3.po \
ui/tl_PH/pgadmin3.po \
ui/tr_TR/pgadmin3.po \
ui/ur_PK/pgadmin3.po \
ui/zh_CN/pgadmin3.po \
ui/zh_TW/pgadmin3.po

##############################################################################
#
# Note, Only include translations above and below that are officially 
# published. These are listed at: 
#                              http://www.pgadmin.org/pgadmin3/translation.php
#
##############################################################################

nobase_dist_pkgdata_DATA = ui/common/*.xrc \
ui/wxstd.mo \
ui/pgadmin3.lng \
ui/af_ZA/pgadmin3.mo \
ui/ar_SA/pgadmin3.mo \
ui/cs_CZ/pgadmin3.mo \
ui/da_DK/pgadmin3.mo \
ui/de_DE/pgadmin3.mo \
ui/es_ES/pgadmin3.mo \
ui/fa_IR/pgadmin3.mo \
ui/fr_FR/pgadmin3.mo \
ui/hr_HR/pgadmin3.mo \
ui/hu_HU/pgadmin3.mo \
ui/id_ID/pgadmin3.mo \
ui/is_IS/pgadmin3.mo \
ui/it_IT/pgadmin3.mo \
ui/ja_JP/pgadmin3.mo \
ui/nb_NO/pgadmin3.mo \
ui/pl_PL/pgadmin3.mo \
ui/pt_BR/pgadmin3.mo \
ui/ro_RO/pgadmin3.mo \
ui/ru_RU/pgadmin3.mo \
ui/sd_IN/pgadmin3.mo \
ui/sv_SE/pgadmin3.mo \
ui/tl_PH/pgadmin3.mo \
ui/tr_TR/pgadmin3.mo \
ui/ur_PK/pgadmin3.mo \
ui/zh_CN/pgadmin3.mo \
ui/zh_TW/pgadmin3.mo \
ui/de_DE/wxstd.mo \
ui/fr_FR/wxstd.mo \
ui/id_ID/wxstd.mo \
ui/it_IT/wxstd.mo \
ui/ja_JP/wxstd.mo \
ui/pl_PL/wxstd.mo \
ui/tr_TR/wxstd.mo \
ui/zh_CN/wxstd.mo

AM_CXXFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" -Wall -g -I$(top_srcdir)/src/include -I$(WX_HOME)
