#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: Makefile.am 5019 2006-02-21 15:29:07Z dpage $
# Copyright (C) 2002 - 2006, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - src/include/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/copyright.h \
	$(srcdir)/include/ctlSecurityPanel.h \
	$(srcdir)/include/dlgAddFavourite.h \
	$(srcdir)/include/dlgAggregate.h \
	$(srcdir)/include/dlgCast.h \
	$(srcdir)/include/dlgCheck.h \
	$(srcdir)/include/dlgClasses.h \
	$(srcdir)/include/dlgColumn.h \
	$(srcdir)/include/dlgConnect.h \
	$(srcdir)/include/dlgConversion.h \
	$(srcdir)/include/dlgDatabase.h \
	$(srcdir)/include/dlgDomain.h \
	$(srcdir)/include/dlgEditGridOptions.h \
	$(srcdir)/include/dlgForeignKey.h \
	$(srcdir)/include/dlgFunction.h \
	$(srcdir)/include/dlgGroup.h \
	$(srcdir)/include/dlgHbaConfig.h \
	$(srcdir)/include/dlgIndex.h \
	$(srcdir)/include/dlgIndexConstraint.h \
	$(srcdir)/include/dlgLanguage.h \
	$(srcdir)/include/dlgMainConfig.h \
	$(srcdir)/include/dlgManageFavourites.h \
	$(srcdir)/include/dlgOperator.h \
	$(srcdir)/include/dlgPgpassConfig.h \
	$(srcdir)/include/dlgProperty.h \
	$(srcdir)/include/dlgRole.h \
	$(srcdir)/include/dlgRule.h \
	$(srcdir)/include/dlgSchema.h \
	$(srcdir)/include/dlgSelectConnection.h \
	$(srcdir)/include/dlgSequence.h \
	$(srcdir)/include/dlgServer.h \
	$(srcdir)/include/dlgTable.h \
	$(srcdir)/include/dlgTablespace.h \
	$(srcdir)/include/dlgTrigger.h \
	$(srcdir)/include/dlgType.h \
	$(srcdir)/include/dlgUser.h \
	$(srcdir)/include/dlgView.h \
	$(srcdir)/include/explainCanvas.h \
	$(srcdir)/include/favourites.h \
	$(srcdir)/include/frmAbout.h \
	$(srcdir)/include/frmBackup.h \
	$(srcdir)/include/frmConfig.h \
	$(srcdir)/include/frmEditGrid.h \
	$(srcdir)/include/frmExport.h \
	$(srcdir)/include/frmGrantWizard.h \
	$(srcdir)/include/frmHbaConfig.h \
	$(srcdir)/include/frmHelp.h \
	$(srcdir)/include/frmHint.h \
	$(srcdir)/include/frmIndexcheck.h \
	$(srcdir)/include/frmMain.h \
	$(srcdir)/include/frmMainConfig.h \
	$(srcdir)/include/frmMaintenance.h \
	$(srcdir)/include/frmOptions.h \
	$(srcdir)/include/frmPassword.h \
	$(srcdir)/include/frmPgpassConfig.h \
	$(srcdir)/include/frmQuery.h \
	$(srcdir)/include/frmReport.h \
	$(srcdir)/include/frmRestore.h \
	$(srcdir)/include/frmSplash.h \
	$(srcdir)/include/frmStatus.h \
	$(srcdir)/include/frmUpdate.h \
	$(srcdir)/include/md5.h \
	$(srcdir)/include/menu.h \
	$(srcdir)/include/misc.h \
	$(srcdir)/include/pgAdmin3.h \
	$(srcdir)/include/pgAggregate.h \
	$(srcdir)/include/pgCast.h \
	$(srcdir)/include/pgCheck.h \
	$(srcdir)/include/pgCollection.h \
	$(srcdir)/include/pgColumn.h \
	$(srcdir)/include/pgConn.h \
	$(srcdir)/include/pgConstraints.h \
	$(srcdir)/include/pgConversion.h \
	$(srcdir)/include/pgDatabase.h \
	$(srcdir)/include/pgDatatype.h \
	$(srcdir)/include/pgDefs.h \
	$(srcdir)/include/pgDomain.h \
	$(srcdir)/include/pgForeignKey.h \
	$(srcdir)/include/pgFunction.h \
	$(srcdir)/include/pgGroup.h \
	$(srcdir)/include/pgIndex.h \
	$(srcdir)/include/pgIndexConstraint.h \
	$(srcdir)/include/pgLanguage.h \
	$(srcdir)/include/pgObject.h \
	$(srcdir)/include/pgOperator.h \
	$(srcdir)/include/pgOperatorClass.h \
	$(srcdir)/include/pgRole.h \
	$(srcdir)/include/pgRule.h \
	$(srcdir)/include/pgSchema.h \
	$(srcdir)/include/pgSequence.h \
	$(srcdir)/include/pgServer.h \
	$(srcdir)/include/pgSet.h \
	$(srcdir)/include/pgTable.h \
	$(srcdir)/include/pgTablespace.h \
	$(srcdir)/include/pgTrigger.h \
	$(srcdir)/include/pgType.h \
	$(srcdir)/include/pgUser.h \
	$(srcdir)/include/pgView.h \
	$(srcdir)/include/pgconfig.h \
	$(srcdir)/include/pgfeatures.h \
	$(srcdir)/include/postgres.h \
	$(srcdir)/include/precomp.h \
	$(srcdir)/include/sysLogger.h \
	$(srcdir)/include/sysProcess.h \
	$(srcdir)/include/sysSettings.h \
	$(srcdir)/include/update.h \
	$(srcdir)/include/utffile.h \
	$(srcdir)/include/version.h \
	$(srcdir)/include/wxgridsel.h

EXTRA_DIST += \
        $(srcdir)/include/module.mk

include $(srcdir)/include/base/module.mk
include $(srcdir)/include/ctl/module.mk
include $(srcdir)/include/images/module.mk
include $(srcdir)/include/nodes/module.mk
include $(srcdir)/include/parser/module.mk

