#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/pgscript/statements/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	pgscript/statements/pgsAssertStmt.cpp \
	pgscript/statements/pgsBreakStmt.cpp \
	pgscript/statements/pgsContinueStmt.cpp \
	pgscript/statements/pgsDeclareRecordStmt.cpp \
	pgscript/statements/pgsExpressionStmt.cpp \
	pgscript/statements/pgsIfStmt.cpp \
	pgscript/statements/pgsPrintStmt.cpp \
	pgscript/statements/pgsProgram.cpp \
	pgscript/statements/pgsStmt.cpp \
	pgscript/statements/pgsStmtList.cpp \
	pgscript/statements/pgsWhileStmt.cpp

EXTRA_DIST += \
	pgscript/statements/module.mk

