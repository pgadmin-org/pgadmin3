#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# Copyright (C) 2002 - 2005, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# Makefile.am - Makefile for *nix systems
#
#######################################################################

bin_PROGRAMS = pgadmin3

#  db/keywords.c  needs to be added

pgadmin3_SOURCES = pgAdmin3.cpp \
db/pgConn.cpp db/pgSet.cpp db/keywords.c \
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
schema/pgTable.cpp schema/pgTablespace.cpp schema/pgTrigger.cpp \
schema/pgType.cpp schema/pgUser.cpp schema/pgView.cpp \
schema/pgDatatype.cpp \
agent/pgaAgent.cpp agent/pgaJob.cpp \
agent/pgaStep.cpp agent/pgaSchedule.cpp \
agent/dlgJob.cpp agent/dlgSchedule.cpp agent/dlgStep.cpp \
ui/xh_sqlbox.cpp ui/xh_calb.cpp ui/xh_timespin.cpp ui/xh_ctlcombo.cpp \
ui/ctlSecurityPanel.cpp ui/dlgClasses.cpp ui/ctlComboBox.cpp \
ui/calbox.cpp ui/timespin.cpp ui/ctlListView.cpp \
ui/ctlSQLBox.cpp ui/ctlSQLResult.cpp ui/frmExport.cpp \
ui/dlgEditGridOptions.cpp ui/events.cpp ui/frmAbout.cpp \
ui/frmIndexcheck.cpp ui/dlgConnect.cpp ui/frmMain.cpp \
ui/frmOptions.cpp ui/frmPassword.cpp ui/frmQuery.cpp \
ui/frmHelp.cpp ui/frmSplash.cpp ui/frmMaintenance.cpp \
ui/frmBackup.cpp ui/frmRestore.cpp ui/frmGrantWizard.cpp \
ui/frmEditGrid.cpp ui/dlgProperty.cpp ui/dlgUser.cpp \
ui/dlgServer.cpp ui/dlgGroup.cpp ui/dlgDatabase.cpp ui/dlgLanguage.cpp \
ui/dlgSchema.cpp ui/dlgDomain.cpp ui/dlgTable.cpp ui/dlgTablespace.cpp \
ui/dlgColumn.cpp ui/dlgIndex.cpp ui/dlgFunction.cpp \
ui/dlgView.cpp ui/dlgRule.cpp ui/dlgOperator.cpp \
ui/dlgAggregate.cpp ui/dlgCast.cpp ui/dlgConversion.cpp \
ui/dlgIndexConstraint.cpp ui/dlgForeignKey.cpp \
ui/dlgSequence.cpp ui/dlgTrigger.cpp ui/dlgType.cpp \
ui/dlgCheck.cpp ui/frmStatus.cpp utils/misc.cpp \
ui/dlgHbaConfig.cpp ui/dlgMainConfig.cpp ui/frmUpdate.cpp \
ui/frmConfig.cpp ui/frmHbaConfig.cpp ui/frmMainConfig.cpp \
utils/sysLogger.cpp utils/sysSettings.cpp \
utils/utffile.cpp utils/pgconfig.cpp utils/update.cpp \
ui/common/xrcDialogs.cpp

noinst_HEADERS = include/copyright.h include/version.h include/ctlSQLBox.h \
include/ctlSQLResult.h include/ctlListView.h \
include/parser/parse.h include/parser/keywords.h include/nodes/parsenodes.h \
include/postgres.h include/pgDefs.h include/pgDatatype.h \
include/ctlSecurityPanel.h include/dlgClasses.h include/frmGrantWizard.h \
include/dlgCheck.h include/dlgColumn.h include/dlgServer.h \
include/dlgDatabase.h include/dlgDomain.h include/frmExport.h \
include/dlgForeignKey.h include/dlgFunction.h include/dlgGroup.h \
include/dlgIndexConstraint.h include/dlgIndex.h \
include/dlgLanguage.h include/dlgProperty.h include/dlgType.h \
include/dlgView.h include/dlgRule.h include/dlgOperator.h \
include/dlgAggregate.h include/dlgCast.h include/dlgConversion.h \
include/dlgSequence.h include/dlgSchema.h include/dlgTable.h include/dlgTablespace.h \
include/dlgOperator.h include/dlgView.h include/dlgRule.h \
include/dlgTrigger.h include/dlgUser.h include/frmAbout.h \
include/dlgConnect.h include/frmHelp.h \
include/frmEditGrid.h include/dlgEditGridOptions.h include/frmMain.h \
include/frmOptions.h include/frmPassword.h include/frmIndexcheck.h \
include/frmQuery.h include/frmSplash.h \
include/frmStatus.h include/frmMaintenance.h \
include/frmBackup.h include/frmRestore.h \
include/misc.h include/pgfeatures.h include/pgAdmin3.h include/pgAggregate.h \
include/pgCast.h include/pgCheck.h include/pgCollection.h \
include/pgColumn.h include/pgConn.h include/pgTablespace.h \
include/pgConstraints.h include/pgConversion.h \
include/pgDatabase.h include/pgDomain.h \
include/pgForeignKey.h include/pgFunction.h \
include/pgGroup.h include/pgIndexConstraint.h \
include/pgIndex.h include/pgLanguage.h include/pgObject.h \
include/pgOperatorClass.h include/pgOperator.h \
include/pgRule.h include/pgSchema.h include/pgSequence.h \
include/pgServer.h include/pgSet.h include/pgTable.h \
include/pgTrigger.h include/pgType.h include/pgUser.h \
include/pgView.h include/sysLogger.h include/utffile.h \
include/calbox.h include/timespin.h include/ctlComboBox.h \
include/xh_timespin.h include/xh_calb.h include/xh_sqlbox.h include/xh_ctlcombo.h \
include/sysSettings.h include/wxgridsel.h include/menu.h\
include/pgconfig.h include/update.h include/frmUpdate.h \
include/dlgHbaConfig.h include/dlgMainConfig.h \
include/frmConfig.h include/frmHbaConfig.h include/frmMainConfig.h \
agent/include/pgaAgent.h agent/include/pgaJob.h \
agent/include/pgaStep.h agent/include/pgaSchedule.h\
agent/include/dlgJob.h agent/include/dlgStep.h agent/include/dlgSchedule.h
  
EXTRA_DIST = pgAdmin3.dsp pgAdmin3.dsw \
include/images/*.xpm \
include/images/pgAdmin3.ico \
ui/af_ZA/pgadmin3.po \
ui/ar_SA/pgadmin3.po \
ui/ca_ES/pgadmin3.po \
ui/da_DK/pgadmin3.po \
ui/de_DE/pgadmin3.po \
ui/es_ES/pgadmin3.po \
ui/fi_FI/pgadmin3.po \
ui/fr_FR/pgadmin3.po \
ui/hu_HU/pgadmin3.po \
ui/id_ID/pgadmin3.po \
ui/it_IT/pgadmin3.po \
ui/ja_JP/pgadmin3.po \
ui/lv_LV/pgadmin3.po \
ui/nl_NL/pgadmin3.po \
ui/pl_PL/pgadmin3.po \
ui/pt_PT/pgadmin3.po \
ui/ru_RU/pgadmin3.po \
ui/sk_SK/pgadmin3.po \
ui/sl_SI/pgadmin3.po \
ui/sr_YU/pgadmin3.po \
ui/tr_TR/pgadmin3.po \
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
ui/pg_settings.csv \
ui/af_ZA/pgadmin3.mo \
ui/ar_SA/pgadmin3.mo \
ui/ca_ES/pgadmin3.mo \
ui/da_DK/pgadmin3.mo \
ui/de_DE/pgadmin3.mo \
ui/es_ES/pgadmin3.mo \
ui/fi_FI/pgadmin3.mo \
ui/fr_FR/pgadmin3.mo \
ui/hu_HU/pgadmin3.mo \
ui/id_ID/pgadmin3.mo \
ui/it_IT/pgadmin3.mo \
ui/ja_JP/pgadmin3.mo \
ui/lv_LV/pgadmin3.mo \
ui/nl_NL/pgadmin3.mo \
ui/pl_PL/pgadmin3.mo \
ui/pt_PT/pgadmin3.mo \
ui/ru_RU/pgadmin3.mo \
ui/sk_SK/pgadmin3.mo \
ui/sl_SI/pgadmin3.mo \
ui/sr_YU/pgadmin3.mo \
ui/tr_TR/pgadmin3.mo \
ui/zh_CN/pgadmin3.mo \
ui/zh_TW/pgadmin3.mo \
ui/af_ZA/wxstd.mo \
ui/ca_ES/wxstd.mo \
ui/da_DK/wxstd.mo \
ui/de_DE/wxstd.mo \
ui/es_ES/wxstd.mo \
ui/fi_FI/wxstd.mo \
ui/fr_FR/wxstd.mo \
ui/hu_HU/wxstd.mo \
ui/id_ID/wxstd.mo \
ui/it_IT/wxstd.mo \
ui/ja_JP/wxstd.mo \
ui/nl_NL/wxstd.mo \
ui/pl_PL/wxstd.mo \
ui/ru_RU/wxstd.mo \
ui/sl_SI/wxstd.mo \
ui/tr_TR/wxstd.mo \
ui/zh_CN/wxstd.mo \
ui/zh_TW/wxstd.mo

AM_CXXFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" -Wall -g -I$(top_srcdir)/src/include -I$(top_srcdir)/src/agent/include
AM_CPPFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" -Wall -g -I$(top_srcdir)/src/include -I$(top_srcdir)/src/agent/include
AM_CFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" -Wall -g -I$(top_srcdir)/src/include -I$(top_srcdir)/src/agent/include
