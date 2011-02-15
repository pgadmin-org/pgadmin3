#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/pgscript/statements/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/pgscript/statements/pgsAssertStmt.cpp \
	$(srcdir)/pgscript/statements/pgsBreakStmt.cpp \
	$(srcdir)/pgscript/statements/pgsContinueStmt.cpp \
	$(srcdir)/pgscript/statements/pgsDeclareRecordStmt.cpp \
	$(srcdir)/pgscript/statements/pgsExpressionStmt.cpp \
	$(srcdir)/pgscript/statements/pgsIfStmt.cpp \
	$(srcdir)/pgscript/statements/pgsPrintStmt.cpp \
	$(srcdir)/pgscript/statements/pgsProgram.cpp \
	$(srcdir)/pgscript/statements/pgsStmt.cpp \
	$(srcdir)/pgscript/statements/pgsStmtList.cpp \
	$(srcdir)/pgscript/statements/pgsWhileStmt.cpp

EXTRA_DIST += \
	$(srcdir)/pgscript/statements/module.mk

