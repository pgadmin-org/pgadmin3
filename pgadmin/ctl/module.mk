#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/ctl/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
		ctl/calbox.cpp \
        ctl/ctlAuiNotebook.cpp \
        ctl/ctlCheckTreeView.cpp \
        ctl/ctlColourPicker.cpp \
        ctl/ctlComboBox.cpp \
        ctl/ctlListView.cpp \
        ctl/ctlMenuToolbar.cpp \
        ctl/ctlSQLBox.cpp \
        ctl/ctlSQLGrid.cpp \
        ctl/ctlSQLResult.cpp \
        ctl/ctlDefaultSecurityPanel.cpp \
        ctl/ctlSeclabelPanel.cpp \
        ctl/ctlSecurityPanel.cpp \
        ctl/ctlTree.cpp \
		ctl/ctlProgressStatusBar.cpp \
        ctl/explainCanvas.cpp \
        ctl/explainShape.cpp \
        ctl/timespin.cpp \
        ctl/xh_calb.cpp \
        ctl/xh_ctlcolourpicker.cpp \
        ctl/xh_ctlcombo.cpp \
        ctl/xh_ctlchecktreeview.cpp \
        ctl/xh_ctltree.cpp \
        ctl/xh_sqlbox.cpp \
        ctl/xh_timespin.cpp

EXTRA_DIST += \
        ctl/module.mk


