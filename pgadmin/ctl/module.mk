#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2010, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/ctl/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
		$(srcdir)/ctl/calbox.cpp \
        $(srcdir)/ctl/ctlAuiNotebook.cpp \
        $(srcdir)/ctl/ctlCheckTreeView.cpp \
        $(srcdir)/ctl/ctlColourPicker.cpp \
        $(srcdir)/ctl/ctlComboBox.cpp \
        $(srcdir)/ctl/ctlListView.cpp \
        $(srcdir)/ctl/ctlMenuToolbar.cpp \
        $(srcdir)/ctl/ctlSQLBox.cpp \
        $(srcdir)/ctl/ctlSQLGrid.cpp \
        $(srcdir)/ctl/ctlSQLResult.cpp \
        $(srcdir)/ctl/ctlDefaultSecurityPanel.cpp \
        $(srcdir)/ctl/ctlSecurityPanel.cpp \
        $(srcdir)/ctl/ctlTree.cpp \
        $(srcdir)/ctl/explainCanvas.cpp \
        $(srcdir)/ctl/explainShape.cpp \
        $(srcdir)/ctl/timespin.cpp \
        $(srcdir)/ctl/xh_calb.cpp \
        $(srcdir)/ctl/xh_ctlcolourpicker.cpp \
        $(srcdir)/ctl/xh_ctlcombo.cpp \
        $(srcdir)/ctl/xh_ctlchecktreeview.cpp \
        $(srcdir)/ctl/xh_ctltree.cpp \
        $(srcdir)/ctl/xh_sqlbox.cpp \
        $(srcdir)/ctl/xh_timespin.cpp

EXTRA_DIST += \
        $(srcdir)/ctl/module.mk


