#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: module.mk 5828 2007-01-04 16:41:08Z dpage $
# Copyright (C) 2002 - 2007, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - pgadmin/debugger/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/debugger/debugger.cpp \
	$(srcdir)/debugger/wsBreakPoint.cpp \
	$(srcdir)/debugger/wsCodeWindow.cpp \
	$(srcdir)/debugger/wsDbresult.cpp \
	$(srcdir)/debugger/wsDirectdbg.cpp \
	$(srcdir)/debugger/wsMainFrame.cpp \
	$(srcdir)/debugger/wsMessageWindow.cpp \
	$(srcdir)/debugger/wsPgconn.cpp \
	$(srcdir)/debugger/wsPgthread.cpp \
	$(srcdir)/debugger/wsResultGrid.cpp \
	$(srcdir)/debugger/wsResultset.cpp \
	$(srcdir)/debugger/wsRichWindow.cpp \
	$(srcdir)/debugger/wsStackWindow.cpp \
	$(srcdir)/debugger/wsTabWindow.cpp \
	$(srcdir)/debugger/wsTargetInfo.cpp \
	$(srcdir)/debugger/wsVarWindow.cpp \
	$(srcdir)/debugger/wsWaitingDialog.cpp

EXTRA_DIST += \
	$(srcdir)/debugger/module.mk


