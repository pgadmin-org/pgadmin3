#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/pgscript/statements/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	include/pgscript/statements/pgsAssertStmt.h \
	include/pgscript/statements/pgsBreakStmt.h \
	include/pgscript/statements/pgsContinueStmt.h \
	include/pgscript/statements/pgsDeclareRecordStmt.h \
	include/pgscript/statements/pgsExpressionStmt.h \
	include/pgscript/statements/pgsIfStmt.h \
	include/pgscript/statements/pgsPrintStmt.h \
	include/pgscript/statements/pgsProgram.h \
	include/pgscript/statements/pgsStatements.h \
	include/pgscript/statements/pgsStmt.h \
	include/pgscript/statements/pgsStmtList.h \
	include/pgscript/statements/pgsWhileStmt.h

EXTRA_DIST += \
	include/pgscript/statements/module.mk

