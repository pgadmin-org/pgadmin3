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
	$(srcdir)/debugger/ctlCodeWindow.cpp \
	$(srcdir)/debugger/ctlMessageWindow.cpp \
	$(srcdir)/debugger/ctlResultGrid.cpp \
	$(srcdir)/debugger/ctlRichWindow.cpp \
	$(srcdir)/debugger/ctlStackWindow.cpp \
	$(srcdir)/debugger/ctlTabWindow.cpp \
	$(srcdir)/debugger/ctlVarWindow.cpp \
	$(srcdir)/debugger/dbgBreakPoint.cpp \
	$(srcdir)/debugger/dbgDbResult.cpp \
	$(srcdir)/debugger/dbgDirect.cpp \
 	$(srcdir)/debugger/dbgPgConn.cpp \
	$(srcdir)/debugger/dbgPgThread.cpp \
	$(srcdir)/debugger/dbgResultset.cpp \
 	$(srcdir)/debugger/dbgTargetInfo.cpp \
	$(srcdir)/debugger/debugger.cpp \
	$(srcdir)/debugger/frmDebugger.cpp

EXTRA_DIST += \
	$(srcdir)/debugger/module.mk


