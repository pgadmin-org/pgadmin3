#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: module.mk 5828 2007-01-04 16:41:08Z dpage $
# Copyright (C) 2002 - 2007, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - debugger/include/ Makefile fragment
#
#######################################################################

debugger_SOURCES += \
	$(srcdir)/include/debugger.h \
	$(srcdir)/include/debuggerMenu.h \
	$(srcdir)/include/wsBreakPoint.h \
	$(srcdir)/include/wsCodeWindow.h \
	$(srcdir)/include/wsConnProp.h \
	$(srcdir)/include/wsConsole.h \
	$(srcdir)/include/wsConst.h \
	$(srcdir)/include/wsDbresult.h \
	$(srcdir)/include/wsDirectdbg.h \
	$(srcdir)/include/wsMainFrame.h \
	$(srcdir)/include/wsMessageWindow.h \
	$(srcdir)/include/wsPgconn.h \
	$(srcdir)/include/wsPgthread.h \
	$(srcdir)/include/wsQueryWindow.h \
	$(srcdir)/include/wsResultGrid.h \
	$(srcdir)/include/wsResultset.h \
	$(srcdir)/include/wsRichWindow.h \
	$(srcdir)/include/wsStackWindow.h \
	$(srcdir)/include/wsTabWindow.h \
	$(srcdir)/include/wsTargetInfo.h \
	$(srcdir)/include/wsVarWindow.h \
	$(srcdir)/include/wsWaitingDialog.h

EXTRA_DIST += \
	$(srcdir)/include/module.mk


include $(srcdir)/include/images/module.mk

