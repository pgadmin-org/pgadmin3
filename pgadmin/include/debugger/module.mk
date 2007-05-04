#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: module.mk 5828 2007-01-04 16:41:08Z dpage $
# Copyright (C) 2002 - 2007, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - pgadmin/debugger/include/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/debugger/debugger.h \
	$(srcdir)/include/debugger/wsBreakPoint.h \
	$(srcdir)/include/debugger/wsCodeWindow.h \
	$(srcdir)/include/debugger/wsConnProp.h \
	$(srcdir)/include/debugger/wsConst.h \
	$(srcdir)/include/debugger/wsDbresult.h \
	$(srcdir)/include/debugger/wsDirectdbg.h \
	$(srcdir)/include/debugger/wsMainFrame.h \
	$(srcdir)/include/debugger/wsMessageWindow.h \
	$(srcdir)/include/debugger/wsPgconn.h \
	$(srcdir)/include/debugger/wsPgthread.h \
	$(srcdir)/include/debugger/wsResultGrid.h \
	$(srcdir)/include/debugger/wsResultset.h \
	$(srcdir)/include/debugger/wsRichWindow.h \
	$(srcdir)/include/debugger/wsStackWindow.h \
	$(srcdir)/include/debugger/wsTabWindow.h \
	$(srcdir)/include/debugger/wsTargetInfo.h \
	$(srcdir)/include/debugger/wsVarWindow.h

EXTRA_DIST += \
	$(srcdir)/include/debugger/module.mk


