#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: Makefile.am 5019 2006-02-21 15:29:07Z dpage $
# Copyright (C) 2002 - 2006, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - src/include/ctl/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/ctl/calbox.h \
	$(srcdir)/include/ctl/ctlComboBox.h \
	$(srcdir)/include/ctl/ctlListView.h \
	$(srcdir)/include/ctl/ctlSQLBox.h \
	$(srcdir)/include/ctl/ctlSQLResult.h \
	$(srcdir)/include/ctl/ctlTree.h \
	$(srcdir)/include/ctl/timespin.h \
	$(srcdir)/include/ctl/xh_calb.h \
	$(srcdir)/include/ctl/xh_ctlcombo.h \
	$(srcdir)/include/ctl/xh_ctltree.h \
	$(srcdir)/include/ctl/xh_sqlbox.h \
	$(srcdir)/include/ctl/xh_timespin.h

EXTRA_DIST += \
        $(srcdir)/include/ctl/module.mk

