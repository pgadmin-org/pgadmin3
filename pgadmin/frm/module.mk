#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/frm/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	frm/events.cpp \
	frm/frmAbout.cpp \
	frm/frmBackup.cpp \
	frm/frmBackupGlobals.cpp \
	frm/frmBackupServer.cpp \
	frm/frmConfig.cpp \
	frm/frmDatabaseDesigner.cpp \
	frm/frmEditGrid.cpp \
	frm/frmExport.cpp \
	frm/frmGrantWizard.cpp \
	frm/frmHbaConfig.cpp \
	frm/frmHint.cpp \
	frm/frmImport.cpp \
	frm/frmMain.cpp \
	frm/frmMainConfig.cpp \
	frm/frmMaintenance.cpp \
	frm/frmOptions.cpp \
	frm/frmPassword.cpp \
	frm/frmPgpassConfig.cpp \
	frm/frmQuery.cpp \
	frm/frmReport.cpp \
	frm/frmRestore.cpp \
	frm/frmSplash.cpp \
	frm/frmStatus.cpp \
	frm/plugins.cpp

EXTRA_DIST += \
    frm/module.mk
