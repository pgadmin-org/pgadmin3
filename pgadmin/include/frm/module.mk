#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: module.mk 5466 2006-10-12 09:31:39Z dpage $
# Copyright (C) 2002 - 2007, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - src/include/frm/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/frm/events.h \
	$(srcdir)/include/frm/frmAbout.h \
	$(srcdir)/include/frm/frmBackup.h \
	$(srcdir)/include/frm/frmConfig.h \
	$(srcdir)/include/frm/frmEditGrid.h \
	$(srcdir)/include/frm/frmExport.h \
  	$(srcdir)/include/frm/frmGrantWizard.h \
  	$(srcdir)/include/frm/frmHbaConfig.h \
  	$(srcdir)/include/frm/frmHelp.h \
	$(srcdir)/include/frm/frmHint.h \
  	$(srcdir)/include/frm/frmIndexcheck.h \
	$(srcdir)/include/frm/frmMain.h \
	$(srcdir)/include/frm/frmMainConfig.h \
	$(srcdir)/include/frm/frmMaintenance.h \
	$(srcdir)/include/frm/frmOptions.h \
	$(srcdir)/include/frm/frmPassword.h \
	$(srcdir)/include/frm/frmPgpassConfig.h \
	$(srcdir)/include/frm/frmQuery.h \
	$(srcdir)/include/frm/frmReport.h \
	$(srcdir)/include/frm/frmRestore.h \
	$(srcdir)/include/frm/frmSplash.h \
	$(srcdir)/include/frm/frmStatus.h \
    $(srcdir)/include/frm/menu.h

EXTRA_DIST += \
    $(srcdir)/include/frm/module.mk

