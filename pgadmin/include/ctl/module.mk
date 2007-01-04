#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2007, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - src/include/ctl/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
    $(srcdir)/include/ctl/calbox.h \
	$(srcdir)/include/ctl/ctlComboBox.h \
	$(srcdir)/include/ctl/ctlListView.h \
	$(srcdir)/include/ctl/ctlSecurityPanel.h \
	$(srcdir)/include/ctl/ctlSQLBox.h \
	$(srcdir)/include/ctl/ctlSQLGrid.h \
	$(srcdir)/include/ctl/ctlSQLResult.h \
	$(srcdir)/include/ctl/ctlTree.h \
    $(srcdir)/include/ctl/explainCanvas.h \
	$(srcdir)/include/ctl/timespin.h \
	$(srcdir)/include/ctl/wxgridsel.h \
	$(srcdir)/include/ctl/xh_calb.h \
	$(srcdir)/include/ctl/xh_ctlcombo.h \
	$(srcdir)/include/ctl/xh_ctltree.h \
	$(srcdir)/include/ctl/xh_sqlbox.h \
	$(srcdir)/include/ctl/xh_timespin.h

EXTRA_DIST += \
    $(srcdir)/include/ctl/module.mk

