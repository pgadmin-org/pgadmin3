#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/debugger/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	debugger/ctlMessageWindow.cpp \
	debugger/ctlResultGrid.cpp \
	debugger/ctlStackWindow.cpp \
	debugger/ctlTabWindow.cpp \
	debugger/ctlVarWindow.cpp \
	debugger/dbgBreakPoint.cpp \
	debugger/dbgController.cpp \
	debugger/dbgEvents.cpp \
	debugger/dbgModel.cpp \
 	debugger/dbgTargetInfo.cpp \
	debugger/debugger.cpp \
	debugger/dlgDirectDbg.cpp \
	debugger/frmDebugger.cpp

EXTRA_DIST += \
	debugger/module.mk


