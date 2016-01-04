#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/slony/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	ui/xrcDialogs.cpp 

TMP_ui += \
	ui/ddPrecisionScaleDialog.xrc \
	ui/ddTableNameDialog.xrc \
	ui/dlgAddFavourite.xrc \
	ui/dlgAggregate.xrc \
	ui/dlgCast.xrc \
	ui/dlgCheck.xrc \
	ui/dlgCollation.xrc \
	ui/dlgColumn.xrc \
	ui/dlgConnect.xrc \
	ui/dlgConversion.xrc \
	ui/dlgDatabase.xrc \
	ui/dlgDirectDbg.xrc \
	ui/dlgDomain.xrc \
	ui/dlgEventTrigger.xrc \
	ui/dlgExtension.xrc \
	ui/dlgEditGridOptions.xrc \
	ui/dlgExtTable.xrc \
	ui/dlgFindReplace.xrc \
	ui/dlgForeignDataWrapper.xrc \
	ui/dlgForeignKey.xrc \
	ui/dlgForeignServer.xrc \
	ui/dlgForeignTable.xrc \
	ui/dlgFunction.xrc \
	ui/dlgGroup.xrc \
	ui/dlgHbaConfig.xrc \
	ui/dlgIndex.xrc \
	ui/dlgIndexConstraint.xrc \
	ui/dlgJob.xrc \
	ui/dlgLanguage.xrc \
	ui/dlgMainConfig.xrc \
	ui/dlgManageFavourites.xrc \
	ui/dlgManageMacros.xrc \
	ui/dlgMoveTablespace.xrc \
	ui/dlgOperator.xrc \
	ui/dlgPackage.xrc \
	ui/dlgPgpassConfig.xrc \
	ui/dlgReassignDropOwned.xrc \
	ui/dlgRepCluster.xrc \
	ui/dlgRepClusterUpgrade.xrc \
	ui/dlgRepListen.xrc \
	ui/dlgRepNode.xrc \
	ui/dlgRepPath.xrc \
	ui/dlgRepSequence.xrc \
	ui/dlgRepSet.xrc \
	ui/dlgRepSetMerge.xrc \
	ui/dlgRepSetMove.xrc \
	ui/dlgRepSubscription.xrc \
	ui/dlgRepTable.xrc \
	ui/dlgRole.xrc \
	ui/dlgRule.xrc \
	ui/dlgSchedule.xrc \
	ui/dlgSchema.xrc \
	ui/dlgSearchObject.xrc \
	ui/dlgSelectConnection.xrc \
	ui/dlgSequence.xrc \
	ui/dlgServer.xrc \
	ui/dlgStep.xrc \
	ui/dlgSynonym.xrc \
	ui/dlgTable.xrc \
	ui/dlgTablespace.xrc \
	ui/dlgTextSearchConfiguration.xrc \
	ui/dlgTextSearchDictionary.xrc \
	ui/dlgTextSearchParser.xrc \
	ui/dlgTextSearchTemplate.xrc \
	ui/dlgTrigger.xrc \
	ui/dlgType.xrc \
	ui/dlgUser.xrc \
	ui/dlgUserMapping.xrc \
	ui/dlgView.xrc \
	ui/frmBackup.xrc \
	ui/frmBackupGlobals.xrc \
	ui/frmBackupServer.xrc \
	ui/frmExport.xrc \
	ui/frmGrantWizard.xrc \
	ui/frmHint.xrc \
	ui/frmImport.xrc \
	ui/frmMaintenance.xrc \
	ui/frmOptions.xrc \
	ui/frmPassword.xrc \
	ui/frmReport.xrc \
	ui/frmRestore.xrc \
	ui/dlgResourceGroup.xrc

EXTRA_DIST += \
	ui/module.mk \
	ui/embed-xrc \
	ui/embed-xrc.bat


