#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/pgscript/statements/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/pgscript/statements/pgsAssertStmt.h \
	$(srcdir)/include/pgscript/statements/pgsBreakStmt.h \
	$(srcdir)/include/pgscript/statements/pgsContinueStmt.h \
	$(srcdir)/include/pgscript/statements/pgsDeclareRecordStmt.h \
	$(srcdir)/include/pgscript/statements/pgsExpressionStmt.h \
	$(srcdir)/include/pgscript/statements/pgsIfStmt.h \
	$(srcdir)/include/pgscript/statements/pgsPrintStmt.h \
	$(srcdir)/include/pgscript/statements/pgsProgram.h \
	$(srcdir)/include/pgscript/statements/pgsStatements.h \
	$(srcdir)/include/pgscript/statements/pgsStmt.h \
	$(srcdir)/include/pgscript/statements/pgsStmtList.h \
	$(srcdir)/include/pgscript/statements/pgsWhileStmt.h

EXTRA_DIST += \
	$(srcdir)/include/pgscript/statements/module.mk

