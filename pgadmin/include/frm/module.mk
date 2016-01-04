#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/frm/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	include/frm/frmAbout.h \
	include/frm/frmBackup.h \
	include/frm/frmBackupGlobals.h \
	include/frm/frmBackupServer.h \
	include/frm/frmConfig.h \
	include/frm/frmDatabaseDesigner.h \
	include/frm/frmEditGrid.h \
	include/frm/frmExport.h \
  	include/frm/frmGrantWizard.h \
  	include/frm/frmHbaConfig.h \
	include/frm/frmHint.h \
	include/frm/frmImport.h \
	include/frm/frmMain.h \
	include/frm/frmMainConfig.h \
	include/frm/frmMaintenance.h \
	include/frm/frmOptions.h \
	include/frm/frmPassword.h \
	include/frm/frmPgpassConfig.h \
	include/frm/frmQuery.h \
	include/frm/frmReport.h \
	include/frm/frmRestore.h \
	include/frm/frmSplash.h \
	include/frm/frmStatus.h \
    	include/frm/menu.h

EXTRA_DIST += \
    include/frm/module.mk

