#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2005, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# Makefile.am - Makefile for *nix systems
#
#######################################################################

bin_PROGRAMS = pgadmin3

pgadmin3_SOURCES = \
$(srcdir)/pgAdmin3.cpp \
\
$(srcdir)/base/base.cpp $(srcdir)/base/appbase.cpp $(srcdir)/base/sysLogger.cpp \
$(srcdir)/base/pgConnBase.cpp $(srcdir)/base/pgSetBase.cpp $(srcdir)/base/factory.cpp \
\
$(srcdir)/main/dlgClasses.cpp $(srcdir)/main/events.cpp \
\
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
$(srcdir)/schema/pgRole.cpp $(srcdir)/schema/pgRule.cpp $(srcdir)/schema/pgSchema.cpp \
$(srcdir)/schema/pgSequence.cpp $(srcdir)/schema/pgServer.cpp \
$(srcdir)/schema/pgTable.cpp $(srcdir)/schema/pgTablespace.cpp $(srcdir)/schema/pgTrigger.cpp \
$(srcdir)/schema/pgType.cpp $(srcdir)/schema/pgUser.cpp $(srcdir)/schema/pgView.cpp \
$(srcdir)/schema/pgDatatype.cpp \
\
$(srcdir)/agent/pgaJob.cpp $(srcdir)/agent/pgaStep.cpp $(srcdir)/agent/pgaSchedule.cpp \
$(srcdir)/agent/dlgJob.cpp $(srcdir)/agent/dlgSchedule.cpp $(srcdir)/agent/dlgStep.cpp \
\
$(srcdir)/ctl/xh_sqlbox.cpp $(srcdir)/ctl/xh_calb.cpp $(srcdir)/ctl/xh_timespin.cpp $(srcdir)/ctl/xh_ctlcombo.cpp \
$(srcdir)/ctl/ctlSecurityPanel.cpp $(srcdir)/ctl/ctlComboBox.cpp $(srcdir)/ctl/ctlTree.cpp \
$(srcdir)/ctl/calbox.cpp $(srcdir)/ctl/timespin.cpp $(srcdir)/ctl/ctlListView.cpp \
$(srcdir)/ctl/ctlSQLBox.cpp $(srcdir)/ctl/ctlSQLResult.cpp \
$(srcdir)/ctl/explainCanvas.cpp $(srcdir)/ctl/explainShape.cpp \
\
$(srcdir)/dlg/dlgEditGridOptions.cpp $(srcdir)/dlg/dlgConnect.cpp \
$(srcdir)/dlg/dlgHbaConfig.cpp $(srcdir)/dlg/dlgMainConfig.cpp $(srcdir)/dlg/dlgSelectConnection.cpp \
\
$(srcdir)/frm/frmExport.cpp $(srcdir)/frm/frmAbout.cpp \
$(srcdir)/frm/frmIndexcheck.cpp  $(srcdir)/frm/frmMain.cpp \
$(srcdir)/frm/frmOptions.cpp $(srcdir)/frm/frmPassword.cpp $(srcdir)/frm/frmQuery.cpp \
$(srcdir)/frm/frmHelp.cpp $(srcdir)/frm/frmSplash.cpp $(srcdir)/frm/frmMaintenance.cpp \
$(srcdir)/frm/frmBackup.cpp $(srcdir)/frm/frmRestore.cpp $(srcdir)/frm/frmGrantWizard.cpp \
$(srcdir)/frm/frmEditGrid.cpp $(srcdir)/frm/frmStatus.cpp $(srcdir)/frm/frmUpdate.cpp \
$(srcdir)/frm/frmConfig.cpp $(srcdir)/frm/frmHbaConfig.cpp $(srcdir)/frm/frmMainConfig.cpp $(srcdir)/frm/frmHint.cpp \
\
$(srcdir)/dlg/dlgProperty.cpp $(srcdir)/dlg/dlgUser.cpp \
$(srcdir)/dlg/dlgServer.cpp $(srcdir)/dlg/dlgGroup.cpp $(srcdir)/dlg/dlgDatabase.cpp $(srcdir)/dlg/dlgLanguage.cpp \
$(srcdir)/dlg/dlgSchema.cpp $(srcdir)/dlg/dlgDomain.cpp $(srcdir)/dlg/dlgTable.cpp $(srcdir)/dlg/dlgTablespace.cpp \
$(srcdir)/dlg/dlgColumn.cpp $(srcdir)/dlg/dlgIndex.cpp $(srcdir)/dlg/dlgFunction.cpp \
$(srcdir)/dlg/dlgView.cpp $(srcdir)/dlg/dlgRule.cpp $(srcdir)/dlg/dlgRole.cpp $(srcdir)/dlg/dlgOperator.cpp \
$(srcdir)/dlg/dlgAggregate.cpp $(srcdir)/dlg/dlgCast.cpp $(srcdir)/dlg/dlgConversion.cpp \
$(srcdir)/dlg/dlgIndexConstraint.cpp $(srcdir)/dlg/dlgForeignKey.cpp \
$(srcdir)/dlg/dlgSequence.cpp $(srcdir)/dlg/dlgTrigger.cpp $(srcdir)/dlg/dlgType.cpp \
$(srcdir)/dlg/dlgCheck.cpp \
\
$(srcdir)/utils/sysSettings.cpp $(srcdir)/utils/sysProcess.cpp \
utils/misc.cpp $(srcdir)/utils/utffile.cpp $(srcdir)/utils/pgconfig.cpp $(srcdir)/utils/update.cpp \
\
$(srcdir)/slony/slCluster.cpp $(srcdir)/slony/slNode.cpp \
$(srcdir)/slony/slPath.cpp $(srcdir)/slony/slListen.cpp $(srcdir)/slony/slSet.cpp \
$(srcdir)/slony/slSequence.cpp $(srcdir)/slony/slTable.cpp $(srcdir)/slony/slSubscription.cpp \
$(srcdir)/slony/dlgRepCluster.cpp $(srcdir)/slony/dlgRepNode.cpp $(srcdir)/slony/dlgRepListen.cpp \
$(srcdir)/slony/dlgRepPath.cpp $(srcdir)/slony/dlgRepSet.cpp $(srcdir)/slony/dlgRepSequence.cpp \
$(srcdir)/slony/dlgRepTable.cpp $(srcdir)/slony/dlgRepSubscription.cpp \
\
$(srcdir)/ui/xrcDialogs.cpp

noinst_HEADERS = \
$(srcdir)/include/base/base.h $(srcdir)/include/base/appbase.h $(srcdir)/include/base/sysLogger.h \
$(srcdir)/include/base/pgDefs.h $(srcdir)/include/base/pgConnBase.h $(srcdir)/include/base/pgSetBase.h \
$(srcdir)/include/base/factory.h $(srcdir)/include/precomp.h \
$(srcdir)/include/copyright.h $(srcdir)/include/version.h\
\
$(srcdir)/include/ctl/ctlSQLBox.h $(srcdir)/include/ctl/ctlSQLResult.h \
$(srcdir)/include/ctl/ctlListView.h $(srcdir)/include/ctl/calbox.h \
$(srcdir)/include/ctl/timespin.h $(srcdir)/include/ctl/ctlComboBox.h \
$(srcdir)/include/ctl/xh_timespin.h $(srcdir)/include/ctl/xh_calb.h $(srcdir)/include/ctl/xh_sqlbox.h \
$(srcdir)/include/ctl/xh_ctlcombo.h $(srcdir)/include/ctl/ctlTree.h \
$(srcdir)/include/parser/parse.h $(srcdir)/include/parser/keywords.h $(srcdir)/include/nodes/parsenodes.h \
$(srcdir)/include/postgres.h $(srcdir)/include/pgDefs.h $(srcdir)/include/pgDatatype.h \
$(srcdir)/include/ctlSecurityPanel.h $(srcdir)/include/dlgClasses.h $(srcdir)/include/frmGrantWizard.h \
$(srcdir)/include/dlgCheck.h $(srcdir)/include/dlgColumn.h $(srcdir)/include/dlgServer.h \
$(srcdir)/include/dlgDatabase.h $(srcdir)/include/dlgDomain.h $(srcdir)/include/frmExport.h \
$(srcdir)/include/dlgForeignKey.h $(srcdir)/include/dlgFunction.h $(srcdir)/include/dlgGroup.h \
$(srcdir)/include/dlgIndexConstraint.h $(srcdir)/include/dlgIndex.h $(srcdir)/include/dlgRole.h \
$(srcdir)/include/dlgLanguage.h $(srcdir)/include/dlgProperty.h $(srcdir)/include/dlgType.h \
$(srcdir)/include/dlgView.h $(srcdir)/include/dlgRule.h $(srcdir)/include/dlgOperator.h \
$(srcdir)/include/dlgAggregate.h $(srcdir)/include/dlgCast.h $(srcdir)/include/dlgConversion.h \
$(srcdir)/include/dlgSequence.h $(srcdir)/include/dlgSchema.h $(srcdir)/include/dlgTable.h $(srcdir)/include/dlgTablespace.h \
$(srcdir)/include/dlgOperator.h $(srcdir)/include/dlgView.h $(srcdir)/include/dlgRule.h \
$(srcdir)/include/dlgTrigger.h $(srcdir)/include/dlgUser.h $(srcdir)/include/frmAbout.h \
$(srcdir)/include/dlgConnect.h $(srcdir)/include/frmHelp.h \
$(srcdir)/include/frmEditGrid.h $(srcdir)/include/dlgEditGridOptions.h $(srcdir)/include/frmMain.h \
$(srcdir)/include/dlgSelectConnection.h \
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
$(srcdir)/include/pgRole.h $(srcdir)/include/pgRule.h $(srcdir)/include/pgSchema.h $(srcdir)/include/pgSequence.h \
$(srcdir)/include/pgServer.h $(srcdir)/include/pgSet.h $(srcdir)/include/pgTable.h \
$(srcdir)/include/pgTrigger.h $(srcdir)/include/pgType.h $(srcdir)/include/pgUser.h \
$(srcdir)/include/pgView.h $(srcdir)/include/sysLogger.h $(srcdir)/include/utffile.h \
$(srcdir)/include/sysSettings.h $(srcdir)/include/sysProcess.h $(srcdir)/include/wxgridsel.h $(srcdir)/include/menu.h\
$(srcdir)/include/pgconfig.h $(srcdir)/include/update.h $(srcdir)/include/frmUpdate.h \
$(srcdir)/include/dlgHbaConfig.h $(srcdir)/include/dlgMainConfig.h $(srcdir)/include/frmHint.h \
$(srcdir)/include/frmConfig.h $(srcdir)/include/frmHbaConfig.h $(srcdir)/include/frmMainConfig.h \
$(srcdir)/include/explainCanvas.h \
$(srcdir)/agent/include/pgaJob.h \
$(srcdir)/agent/include/pgaStep.h $(srcdir)/agent/include/pgaSchedule.h \
$(srcdir)/agent/include/dlgJob.h $(srcdir)/agent/include/dlgStep.h \
$(srcdir)/agent/include/dlgSchedule.h \
$(srcdir)/slony/include/slCluster.h $(srcdir)/slony/include/slNode.h \
$(srcdir)/slony/include/slPath.h $(srcdir)/slony/include/slListen.h \
$(srcdir)/slony/include/slSet.h $(srcdir)/slony/include/slSequence.h \
$(srcdir)/slony/include/slTable.h $(srcdir)/slony/include/slSubscription.h \
$(srcdir)/slony/include/dlgRepCluster.h $(srcdir)/slony/include/dlgRepNode.h \
$(srcdir)/slony/include/dlgRepListen.h $(srcdir)/slony/include/dlgRepPath.h \
$(srcdir)/slony/include/dlgRepSet.h $(srcdir)/slony/include/dlgRepSequence.h \
$(srcdir)/slony/include/dlgRepSubscription.h $(srcdir)/slony/include/dlgRepProperty.h \
$(srcdir)/slony/include/dlgRepTable.h 

  
EXTRA_DIST = \
$(srcdir)/pgAdmin3.dsp \
$(srcdir)/pgAdmin3.dsw \
$(srcdir)/include/images/*.xpm \
$(srcdir)/include/images/pgAdmin3.ico

TMP_ui = \
$(srcdir)/ui/*.xrc

if !APPBUNDLE

nobase_dist_pkgdata_DATA = $(TMP_ui)
AM_CXXFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" -Wall -Wno-non-virtual-dtor -I$(top_srcdir)/src/include -I$(top_srcdir)/src/agent/include -I$(top_srcdir)/src/slony/include
AM_CPPFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" -Wall -Wno-non-virtual-dtor -I$(top_srcdir)/src/include -I$(top_srcdir)/src/agent/include -I$(top_srcdir)/src/slony/include
AM_CFLAGS = -DDATA_DIR=\"$(pkgdatadir)/\" -Wall -I$(top_srcdir)/src/include -I$(top_srcdir)/src/agent/include -I$(top_srcdir)/src/slony/include

# Automake trys to execute install-exec-hook if it appears anywhere in the file, so we need a dummy
# for non-APPBUNDLE cases.
install-exec-hook:

else

nobase_dist_data_DATA = $(TMP_ui)
AM_CXXFLAGS = -Wall -Wno-non-virtual-dtor -I$(top_srcdir)/src/include -I$(top_srcdir)/src/agent/include -I$(top_srcdir)/src/slony/include
AM_CPPFLAGS = -Wall -Wno-non-virtual-dtor -I$(top_srcdir)/src/include -I$(top_srcdir)/src/agent/include -I$(top_srcdir)/src/slony/include
AM_CFLAGS = -Wall -I$(top_srcdir)/src/include -I$(top_srcdir)/src/agent/include -I$(top_srcdir)/src/slony/include
install-exec-hook:
	cd $(bindir) ;\
	mv pgadmin3 tmp ;\
	mv tmp pgAdmin3 ;\
	if test "$(BUILD_DEBUG)" = "no"; then \
		strip pgAdmin3 ;\
	fi

endif
