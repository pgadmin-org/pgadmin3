#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/ctl/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	include/ctl/calbox.h \
	include/ctl/ctlAuiNotebook.h \
	include/ctl/ctlCheckTreeView.h \
	include/ctl/ctlColourPicker.h \
	include/ctl/ctlComboBox.h \
	include/ctl/ctlListView.h \
	include/ctl/ctlMenuToolbar.h \
	include/ctl/ctlDefaultSecurityPanel.h \
	include/ctl/ctlSeclabelPanel.h \
	include/ctl/ctlSecurityPanel.h \
	include/ctl/ctlSQLBox.h \
	include/ctl/ctlSQLGrid.h \
	include/ctl/ctlSQLResult.h \
	include/ctl/ctlProgressStatusBar.h \
	include/ctl/ctlTree.h \
	include/ctl/explainCanvas.h \
	include/ctl/timespin.h \
	include/ctl/wxgridsel.h \
	include/ctl/xh_calb.h \
	include/ctl/xh_ctlcombo.h \
	include/ctl/xh_ctlcolourpicker.h \
	include/ctl/xh_ctlchecktreeview.h \
	include/ctl/xh_ctltree.h \
	include/ctl/xh_sqlbox.h \
	include/ctl/xh_timespin.h

EXTRA_DIST += \
	include/ctl/module.mk


