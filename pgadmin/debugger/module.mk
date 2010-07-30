#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2009, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/debugger/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/debugger/ctlCodeWindow.cpp \
	$(srcdir)/debugger/ctlMessageWindow.cpp \
	$(srcdir)/debugger/ctlResultGrid.cpp \
	$(srcdir)/debugger/ctlStackWindow.cpp \
	$(srcdir)/debugger/ctlTabWindow.cpp \
	$(srcdir)/debugger/ctlVarWindow.cpp \
	$(srcdir)/debugger/dbgBreakPoint.cpp \
	$(srcdir)/debugger/dbgDbResult.cpp \
 	$(srcdir)/debugger/dbgPgConn.cpp \
	$(srcdir)/debugger/dbgPgThread.cpp \
	$(srcdir)/debugger/dbgResultset.cpp \
 	$(srcdir)/debugger/dbgTargetInfo.cpp \
	$(srcdir)/debugger/debugger.cpp \
	$(srcdir)/debugger/dlgDirectDbg.cpp \
	$(srcdir)/debugger/frmDebugger.cpp

EXTRA_DIST += \
	$(srcdir)/debugger/module.mk


