#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2007, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - src/slony/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/ui/xrcDialogs.cpp 

TMP_ui += \
	$(srcdir)/ui/dlgAddFavourite.xrc \
	$(srcdir)/ui/dlgAggregate.xrc \
	$(srcdir)/ui/dlgCast.xrc \
	$(srcdir)/ui/dlgCheck.xrc \
	$(srcdir)/ui/dlgColumn.xrc \
	$(srcdir)/ui/dlgConnect.xrc \
	$(srcdir)/ui/dlgConversion.xrc \
	$(srcdir)/ui/dlgDatabase.xrc \
	$(srcdir)/ui/dlgDomain.xrc \
	$(srcdir)/ui/dlgEditGridOptions.xrc \
	$(srcdir)/ui/dlgFindReplace.xrc \
	$(srcdir)/ui/dlgForeignKey.xrc \
	$(srcdir)/ui/dlgFunction.xrc \
	$(srcdir)/ui/dlgGroup.xrc \
	$(srcdir)/ui/dlgHbaConfig.xrc \
	$(srcdir)/ui/dlgIndex.xrc \
	$(srcdir)/ui/dlgIndexConstraint.xrc \
	$(srcdir)/ui/dlgJob.xrc \
	$(srcdir)/ui/dlgLanguage.xrc \
	$(srcdir)/ui/dlgMainConfig.xrc \
	$(srcdir)/ui/dlgManageFavourites.xrc \
	$(srcdir)/ui/dlgOperator.xrc \
	$(srcdir)/ui/dlgPackage.xrc \
	$(srcdir)/ui/dlgPgpassConfig.xrc \
	$(srcdir)/ui/dlgRepCluster.xrc \
	$(srcdir)/ui/dlgRepClusterUpgrade.xrc \
	$(srcdir)/ui/dlgRepListen.xrc \
	$(srcdir)/ui/dlgRepNode.xrc \
	$(srcdir)/ui/dlgRepPath.xrc \
	$(srcdir)/ui/dlgRepSequence.xrc \
	$(srcdir)/ui/dlgRepSet.xrc \
	$(srcdir)/ui/dlgRepSetMerge.xrc \
	$(srcdir)/ui/dlgRepSetMove.xrc \
	$(srcdir)/ui/dlgRepSubscription.xrc \
	$(srcdir)/ui/dlgRepTable.xrc \
	$(srcdir)/ui/dlgRole.xrc \
	$(srcdir)/ui/dlgRule.xrc \
	$(srcdir)/ui/dlgSchedule.xrc \
	$(srcdir)/ui/dlgSchema.xrc \
	$(srcdir)/ui/dlgSelectConnection.xrc \
	$(srcdir)/ui/dlgSequence.xrc \
	$(srcdir)/ui/dlgServer.xrc \
	$(srcdir)/ui/dlgStep.xrc \
	$(srcdir)/ui/dlgSynonym.xrc \
	$(srcdir)/ui/dlgTable.xrc \
	$(srcdir)/ui/dlgTablespace.xrc \
	$(srcdir)/ui/dlgTrigger.xrc \
	$(srcdir)/ui/dlgType.xrc \
	$(srcdir)/ui/dlgUser.xrc \
	$(srcdir)/ui/dlgView.xrc \
	$(srcdir)/ui/frmAddTableView.xrc \
	$(srcdir)/ui/frmBackup.xrc \
	$(srcdir)/ui/frmBackupGlobals.xrc \
	$(srcdir)/ui/frmBackupServer.xrc \
	$(srcdir)/ui/frmExport.xrc \
	$(srcdir)/ui/frmGrantWizard.xrc \
	$(srcdir)/ui/frmHint.xrc \
	$(srcdir)/ui/frmMaintenance.xrc \
	$(srcdir)/ui/frmOptions.xrc \
	$(srcdir)/ui/frmPassword.xrc \
	$(srcdir)/ui/frmReport.xrc \
	$(srcdir)/ui/frmRestore.xrc \
	$(srcdir)/ui/frmStatus.xrc \
	$(srcdir)/ui/frmUpdate.xrc

EXTRA_DIST += \
        $(srcdir)/ui/module.mk \
	$(srcdir)/ui/embed-xrc \
	$(srcdir)/ui/embed-xrc.bat

