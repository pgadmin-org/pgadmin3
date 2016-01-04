#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/debugger/include/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	include/debugger/dbgController.h \
	include/debugger/dbgModel.h \
	include/debugger/ctlMessageWindow.h \
	include/debugger/ctlResultGrid.h \
	include/debugger/ctlStackWindow.h \
	include/debugger/ctlTabWindow.h \
	include/debugger/ctlVarWindow.h \
	include/debugger/dbgBreakPoint.h \
	include/debugger/dbgConst.h \
	include/debugger/dbgTargetInfo.h \
 	include/debugger/debugger.h \
	include/debugger/dlgDirectDbg.h \
	include/debugger/frmDebugger.h

EXTRA_DIST += \
	include/debugger/module.mk


