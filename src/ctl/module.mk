#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: Makefile.am 5019 2006-02-21 15:29:07Z dpage $
# Copyright (C) 2002 - 2006, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - src/ctl/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/ctl/calbox.cpp \
        $(srcdir)/ctl/ctlComboBox.cpp \
        $(srcdir)/ctl/ctlListView.cpp \
        $(srcdir)/ctl/ctlSQLBox.cpp \
        $(srcdir)/ctl/ctlSQLResult.cpp \
        $(srcdir)/ctl/ctlSecurityPanel.cpp \
        $(srcdir)/ctl/ctlTree.cpp \
        $(srcdir)/ctl/explainCanvas.cpp \
        $(srcdir)/ctl/explainShape.cpp \
        $(srcdir)/ctl/timespin.cpp \
        $(srcdir)/ctl/xh_calb.cpp \
        $(srcdir)/ctl/xh_ctlcombo.cpp \
        $(srcdir)/ctl/xh_ctltree.cpp \
        $(srcdir)/ctl/xh_sqlbox.cpp \
        $(srcdir)/ctl/xh_timespin.cpp

EXTRA_DIST += \
        $(srcdir)/ctl/module.mk

