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

pgadmin3_SOURCES = $(srcdir)/pgAdmin3.cpp \
$(srcdir)/db/pgConn.cpp $(srcdir)/db/pgSet.cpp $(srcdir)/db/keywords.c \
$(srcdir)/schema/pgAggregate.cpp $(srcdir)/schema/pgCast.cpp \
$(srcdir)/schema/pgCheck.cpp $(srcdir)/schema/pgCollection.cpp \
$(srcdir)/schema/pgColumn.cpp $(srcdir)/schema/pgConstraints.cpp \
$(srcdir)/schema/pgConversion.cpp $(srcdir)/schema/pgDatabase.cpp \
$(srcdir)/schema/pgDomain.cpp $(srcdir)/schema/pgForeignKey.cpp \
$(srcdir)/schema/pgFunction.cpp $(srcdir)/schema/pgGroup.cpp \
$(srcdir)/schema/pgIndex.cpp $(srcdir)/schema/pgIndexConstraint.cpp \
$(srcdir)/schema/pgLanguage.cpp $(srcdir)/schema/pgObject.cpp \
$(srcdir)/schema/pgOperator.cpp $(srcdir)/schema/pgOperatorClass.cpp \
$(srcdir)/schema/pgRule.cpp $(srcdir)/schema/pgSchema.cpp \
$(srcdir)/schema/pgSequence.cpp $(srcdir)/schema/pgServer.cpp \
$(srcdir)/schema/pgTable.cpp $(srcdir)/schema/pgTablespace.cpp $(srcdir)/schema/pgTrigger.cpp \
$(srcdir)/schema/pgType.cpp $(srcdir)/schema/pgUser.cpp $(srcdir)/schema/pgView.cpp \
$(srcdir)/schema/pgDatatype.cpp \
$(srcdir)/agent/pgaAgent.cpp $(srcdir)/agent/pgaJob.cpp \
$(srcdir)/agent/pgaStep.cpp $(srcdir)/agent/pgaSchedule.cpp \
$(srcdir)/agent/dlgJob.cpp $(srcdir)/agent/dlgSchedule.cpp $(srcdir)/agent/dlgStep.cpp \
$(srcdir)/ui/xh_sqlbox.cpp $(srcdir)/ui/xh_calb.cpp $(srcdir)/ui/xh_timespin.cpp $(srcdir)/ui/xh_ctlcombo.cpp \
$(srcdir)/ui/ctlSecurityPanel.cpp $(srcdir)/ui/dlgClasses.cpp $(srcdir)/ui/ctlComboBox.cpp \
$(srcdir)/ui/calbox.cpp $(srcdir)/ui/timespin.cpp $(srcdir)/ui/ctlListView.cpp \
$(srcdir)/ui/ctlSQLBox.cpp $(srcdir)/ui/ctlSQLResult.cpp $(srcdir)/ui/frmExport.cpp \
$(srcdir)/ui/dlgEditGridOptions.cpp $(srcdir)/ui/events.cpp $(srcdir)/ui/frmAbout.cpp \
$(srcdir)/ui/frmIndexcheck.cpp $(srcdir)/ui/dlgConnect.cpp $(srcdir)/ui/frmMain.cpp \
$(srcdir)/ui/frmOptions.cpp $(srcdir)/ui/frmPassword.cpp $(srcdir)/ui/frmQuery.cpp \
$(srcdir)/ui/frmHelp.cpp $(srcdir)/ui/frmSplash.cpp $(srcdir)/ui/frmMaintenance.cpp \
$(srcdir)/ui/frmBackup.cpp $(srcdir)/ui/frmRestore.cpp $(srcdir)/ui/frmGrantWizard.cpp \
$(srcdir)/ui/frmEditGrid.cpp $(srcdir)/ui/dlgProperty.cpp $(srcdir)/ui/dlgUser.cpp \
$(srcdir)/ui/dlgServer.cpp $(srcdir)/ui/dlgGroup.cpp $(srcdir)/ui/dlgDatabase.cpp $(srcdir)/ui/dlgLanguage.cpp \
$(srcdir)/ui/dlgSchema.cpp $(srcdir)/ui/dlgDomain.cpp $(srcdir)/ui/dlgTable.cpp $(srcdir)/ui/dlgTablespace.cpp \
$(srcdir)/ui/dlgColumn.cpp $(srcdir)/ui/dlgIndex.cpp $(srcdir)/ui/dlgFunction.cpp \
$(srcdir)/ui/dlgView.cpp $(srcdir)/ui/dlgRule.cpp $(srcdir)/ui/dlgOperator.cpp \
$(srcdir)/ui/dlgAggregate.cpp $(srcdir)/ui/dlgCast.cpp $(srcdir)/ui/dlgConversion.cpp \
$(srcdir)/ui/dlgIndexConstraint.cpp $(srcdir)/ui/dlgForeignKey.cpp \
$(srcdir)/ui/dlgSequence.cpp $(srcdir)/ui/dlgTrigger.cpp $(srcdir)/ui/dlgType.cpp \
$(srcdir)/ui/dlgCheck.cpp $(srcdir)/ui/frmStatus.cpp utils/misc.cpp \
$(srcdir)/ui/dlgHbaConfig.cpp $(srcdir)/ui/dlgMainConfig.cpp $(srcdir)/ui/frmUpdate.cpp \
$(srcdir)/ui/frmConfig.cpp $(srcdir)/ui/frmHbaConfig.cpp $(srcdir)/ui/frmMainConfig.cpp $(srcdir)/ui/frmHint.cpp \
$(srcdir)/utils/sysLogger.cpp $(srcdir)/utils/sysSettings.cpp $(srcdir)/utils/sysProcess.cpp \
$(srcdir)/utils/utffile.cpp $(srcdir)/utils/pgconfig.cpp $(srcdir)/utils/update.cpp \
$(srcdir)/slony/slObject.cpp $(srcdir)/slony/slCluster.cpp $(srcdir)/slony/slNode.cpp \
$(srcdir)/slony/slPath.cpp $(srcdir)/slony/slListen.cpp $(srcdir)/slony/slSet.cpp \
$(srcdir)/slony/slSequence.cpp $(srcdir)/slony/slTable.cpp $(srcdir)/slony/slSubscription.cpp \
$(srcdir)/slony/dlgRepCluster.cpp $(srcdir)/slony/dlgRepNode.cpp $(srcdir)/slony/dlgRepListen.cpp \
$(srcdir)/slony/dlgRepPath.cpp $(srcdir)/slony/dlgRepSet.cpp $(srcdir)/slony/dlgRepSequence.cpp \
$(srcdir)/slony/dlgRepTable.cpp $(srcdir)/slony/dlgRepSubscription.cpp $(srcdir)/slony/slFunctions.cpp \
$(srcdir)/ui/common/xrcDialogs.cpp

noinst_HEADERS = $(srcdir)/include/copyright.h $(srcdir)/include/version.h $(srcdir)/include/ctlSQLBox.h \
$(srcdir)/include/ctlSQLResult.h $(srcdir)/include/ctlListView.h \
$(srcdir)/include/parser/parse.h $(srcdir)/include/parser/keywords.h $(srcdir)/include/nodes/parsenodes.h \
$(srcdir)/include/postgres.h $(srcdir)/include/pgDefs.h $(srcdir)/include/pgDatatype.h \
$(srcdir)/include/ctlSecurityPanel.h $(srcdir)/include/dlgClasses.h $(srcdir)/include/frmGrantWizard.h \
$(srcdir)/include/dlgCheck.h $(srcdir)/include/dlgColumn.h $(srcdir)/include/dlgServer.h \
$(srcdir)/include/dlgDatabase.h $(srcdir)/include/dlgDomain.h $(srcdir)/include/frmExport.h \
$(srcdir)/include/dlgForeignKey.h $(srcdir)/include/dlgFunction.h $(srcdir)/include/dlgGroup.h \
$(srcdir)/include/dlgIndexConstraint.h $(srcdir)/include/dlgIndex.h \
$(srcdir)/include/dlgLanguage.h $(srcdir)/include/dlgProperty.h $(srcdir)/include/dlgType.h \
$(srcdir)/include/dlgView.h $(srcdir)/include/dlgRule.h $(srcdir)/include/dlgOperator.h \
$(srcdir)/include/dlgAggregate.h $(srcdir)/include/dlgCast.h $(srcdir)/include/dlgConversion.h \
$(srcdir)/include/dlgSequence.h $(srcdir)/include/dlgSchema.h $(srcdir)/include/dlgTable.h $(srcdir)/include/dlgTablespace.h \
$(srcdir)/include/dlgOperator.h $(srcdir)/include/dlgView.h $(srcdir)/include/dlgRule.h \
$(srcdir)/include/dlgTrigger.h $(srcdir)/include/dlgUser.h $(srcdir)/include/frmAbout.h \
$(srcdir)/include/dlgConnect.h $(srcdir)/include/frmHelp.h \
$(srcdir)/include/frmEditGrid.h $(srcdir)/include/dlgEditGridOptions.h $(srcdir)/include/frmMain.h \
$(srcdir)/include/frmOptions.h $(srcdir)/include/frmPassword.h $(srcdir)/include/frmIndexcheck.h \
$(srcdir)/include/frmQuery.h $(srcdir)/include/frmSplash.h \
$(srcdir)/include/frmStatus.h $(srcdir)/include/frmMaintenance.h \
$(srcdir)/include/frmBackup.h $(srcdir)/include/frmRestore.h \
$(srcdir)/include/misc.h $(srcdir)/include/pgfeatures.h $(srcdir)/include/pgAdmin3.h $(srcdir)/include/pgAggregate.h \
$(srcdir)/include/pgCast.h $(srcdir)/include/pgCheck.h $(srcdir)/include/pgCollection.h \
$(srcdir)/include/pgColumn.h $(srcdir)/include/pgConn.h $(srcdir)/include/pgTablespace.h \
$(srcdir)/include/pgConstraints.h $(srcdir)/include/pgConversion.h \
$(srcdir)/include/pgDatabase.h $(srcdir)/include/pgDomain.h \
$(srcdir)/include/pgForeignKey.h $(srcdir)/include/pgFunction.h \
$(srcdir)/include/pgGroup.h $(srcdir)/include/pgIndexConstraint.h \
$(srcdir)/include/pgIndex.h $(srcdir)/include/pgLanguage.h $(srcdir)/include/pgObject.h \
$(srcdir)/include/pgOperatorClass.h $(srcdir)/include/pgOperator.h \
$(srcdir)/include/pgRule.h $(srcdir)/include/pgSchema.h $(srcdir)/include/pgSequence.h \
$(srcdir)/include/pgServer.h $(srcdir)/include/pgSet.h $(srcdir)/include/pgTable.h \
$(srcdir)/include/pgTrigger.h $(srcdir)/include/pgType.h $(srcdir)/include/pgUser.h \
$(srcdir)/include/pgView.h $(srcdir)/include/sysLogger.h $(srcdir)/include/utffile.h \
$(srcdir)/include/calbox.h $(srcdir)/include/timespin.h $(srcdir)/include/ctlComboBox.h \
$(srcdir)/include/xh_timespin.h $(srcdir)/include/xh_calb.h $(srcdir)/include/xh_sqlbox.h $(srcdir)/include/xh_ctlcombo.h \
$(srcdir)/include/sysSettings.h $(srcdir)/include/sysProcess.h $(srcdir)/include/wxgridsel.h $(srcdir)/include/menu.h\
$(srcdir)/include/pgconfig.h $(srcdir)/include/update.h $(srcdir)/include/frmUpdate.h \
$(srcdir)/include/dlgHbaConfig.h $(srcdir)/include/dlgMainConfig.h $(srcdir)/include/frmHint.h \
$(srcdir)/include/frmConfig.h $(srcdir)/include/frmHbaConfig.h $(srcdir)/include/frmMainConfig.h \
$(srcdir)/agent/include/pgaAgent.h $(srcdir)/agent/include/pgaJob.h \
$(srcdir)/agent/include/pgaStep.h $(srcdir)/agent/include/pgaSchedule.h \
$(srcdir)/agent/include/dlgJob.h $(srcdir)/agent/include/dlgStep.h \
$(srcdir)/agent/include/dlgSchedule.h $(srcdir)/slony/include/slObject.h \
$(srcdir)/slony/include/slCluster.h $(srcdir)/slony/include/slNode.h \
$(srcdir)/slony/include/slPath.h $(srcdir)/slony/include/slListen.h \
$(srcdir)/slony/include/slSet.h $(srcdir)/slony/include/slSequence.h \
$(srcdir)/slony/include/slTable.h $(srcdir)/slony/include/slSubscription.h \
$(srcdir)/slony/include/dlgRepCluster.h $(srcdir)/slony/include/dlgRepNode.h \
$(srcdir)/slony/include/dlgRepListen.h $(srcdir)/slony/include/dlgRepPath.h \
$(srcdir)/slony/include/dlgRepSet.h $(srcdir)/slony/include/dlgRepSequence.h \
$(srcdir)/slony/include/dlgRepSubscription.h $(srcdir)/slony/include/slFunctions.h

  
EXTRA_DIST = $(srcdir)/pgAdmin3.dsp $(srcdir)/pgAdmin3.dsw \
$(srcdir)/include/images/*.xpm \
$(srcdir)/include/images/pgAdmin3.ico \
$(srcdir)/ui/af_ZA/pgadmin3.po \
$(srcdir)/ui/ar_SA/pgadmin3.po \
$(srcdir)/ui/ca_ES/pgadmin3.po \
$(srcdir)/ui/da_DK/pgadmin3.po \
$(srcdir)/ui/de_DE/pgadmin3.po \
$(srcdir)/ui/es_ES/pgadmin3.po \
$(srcdir)/ui/fi_FI/pgadmin3.po \
$(srcdir)/ui/fr_FR/pgadmin3.po \
$(srcdir)/ui/hu_HU/pgadmin3.po \
$(srcdir)/ui/id_ID/pgadmin3.po \
$(srcdir)/ui/it_IT/pgadmin3.po \
$(srcdir)/ui/ja_JP/pgadmin3.po \
$(srcdir)/ui/lv_LV/pgadmin3.po \
$(srcdir)/ui/nl_NL/pgadmin3.po \
$(srcdir)/ui/pl_PL/pgadmin3.po \
$(srcdir)/ui/pt_PT/pgadmin3.po \
$(srcdir)/ui/ru_RU/pgadmin3.po \
$(srcdir)/ui/sk_SK/pgadmin3.po \
$(srcdir)/ui/sl_SI/pgadmin3.po \
$(srcdir)/ui/sr_YU/pgadmin3.po \
$(srcdir)/ui/tr_TR/pgadmin3.po \
$(srcdir)/ui/zh_CN/pgadmin3.po \
$(srcdir)/ui/zh_TW/pgadmin3.po

##############################################################################
#
# Note, Only include translations above and below that are officially 
# published. These are listed at: 
#                              http://www.pgadmin.org/pgadmin3/translation.php
#
##############################################################################

nobase_dist_pkgdata_DATA = $(srcdir)/ui/common/*.xrc \
$(srcdir)/ui/wxstd.mo \
$(srcdir)/ui/pgadmin3.lng \
$(srcdir)/ui/pg_settings.csv \
$(srcdir)/ui/af_ZA/pgadmin3.mo \
$(srcdir)/ui/ar_SA/pgadmin3.mo \
$(srcdir)/ui/ca_ES/pgadmin3.mo \
$(srcdir)/ui/da_DK/pgadmin3.mo \
$(srcdir)/ui/de_DE/pgadmin3.mo \
$(srcdir)/ui/es_ES/pgadmin3.mo \
$(srcdir)/ui/fi_FI/pgadmin3.mo \
$(srcdir)/ui/fr_FR/pgadmin3.mo \
$(srcdir)/ui/hu_HU/pgadmin3.mo \
$(srcdir)/ui/id_ID/pgadmin3.mo \
$(srcdir)/ui/it_IT/pgadmin3.mo \
$(srcdir)/ui/ja_JP/pgadmin3.mo \
$(srcdir)/ui/lv_LV/pgadmin3.mo \
$(srcdir)/ui/nl_NL/pgadmin3.mo \
$(srcdir)/ui/pl_PL/pgadmin3.mo \
$(srcdir)/ui/pt_PT/pgadmin3.mo \
$(srcdir)/ui/ru_RU/pgadmin3.mo \
$(srcdir)/ui/sk_SK/pgadmin3.mo \
$(srcdir)/ui/sl_SI/pgadmin3.mo \
$(srcdir)/ui/sr_YU/pgadmin3.mo \
$(srcdir)/ui/tr_TR/pgadmin3.mo \
$(srcdir)/ui/zh_CN/pgadmin3.mo \
$(srcdir)/ui/zh_TW/pgadmin3.mo \
$(srcdir)/ui/af_ZA/wxstd.mo \
$(srcdir)/ui/ca_ES/wxstd.mo \
$(srcdir)/ui/da_DK/wxstd.mo \
$(srcdir)/ui/de_DE/wxstd.mo \
$(srcdir)/ui/es_ES/wxstd.mo \
$(srcdir)/ui/fi_FI/wxstd.mo \
$(srcdir)/ui/fr_FR/wxstd.mo \
$(srcdir)/ui/hu_HU/wxstd.mo \
$(srcdir)/ui/id_ID/wxstd.mo \
$(srcdir)/ui/it_IT/wxstd.mo \
$(srcdir)/ui/ja_JP/wxstd.mo \
$(srcdir)/ui/nl_NL/wxstd.mo \
$(srcdir)/ui/pl_PL/wxstd.mo \
$(srcdir)/ui/ru_RU/wxstd.mo \
$(srcdir)/ui/sl_SI/wxstd.mo \
$(srcdir)/ui/tr_TR/wxstd.mo \
$(srcdir)/ui/zh_CN/wxstd.mo \
$(srcdir)/ui/zh_TW/wxstd.mo

AM_CXXFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" -Wall -g -I$(top_srcdir)/src/include -I$(top_srcdir)/src/agent/include -I$(top_srcdir)/src/slony/include
AM_CPPFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" -Wall -g -I$(top_srcdir)/src/include -I$(top_srcdir)/src/agent/include -I$(top_srcdir)/src/slony/include
AM_CFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" -Wall -g -I$(top_srcdir)/src/include -I$(top_srcdir)/src/agent/include -I$(top_srcdir)/src/slony/include

