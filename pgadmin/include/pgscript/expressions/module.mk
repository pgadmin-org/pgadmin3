#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/pgscript/expressions/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/pgscript/expressions/pgsAnd.h \
	$(srcdir)/include/pgscript/expressions/pgsAssign.h \
	$(srcdir)/include/pgscript/expressions/pgsAssignToRecord.h \
	$(srcdir)/include/pgscript/expressions/pgsCast.h \
	$(srcdir)/include/pgscript/expressions/pgsColumns.h \
	$(srcdir)/include/pgscript/expressions/pgsDifferent.h \
	$(srcdir)/include/pgscript/expressions/pgsEqual.h \
	$(srcdir)/include/pgscript/expressions/pgsExecute.h \
	$(srcdir)/include/pgscript/expressions/pgsExpression.h \
	$(srcdir)/include/pgscript/expressions/pgsExpressions.h \
	$(srcdir)/include/pgscript/expressions/pgsGenDate.h \
	$(srcdir)/include/pgscript/expressions/pgsGenDateTime.h \
	$(srcdir)/include/pgscript/expressions/pgsGenDictionary.h \
	$(srcdir)/include/pgscript/expressions/pgsGenInt.h \
	$(srcdir)/include/pgscript/expressions/pgsGenReal.h \
	$(srcdir)/include/pgscript/expressions/pgsGenReference.h \
	$(srcdir)/include/pgscript/expressions/pgsGenRegex.h \
	$(srcdir)/include/pgscript/expressions/pgsGenString.h \
	$(srcdir)/include/pgscript/expressions/pgsGenTime.h \
	$(srcdir)/include/pgscript/expressions/pgsGreater.h \
	$(srcdir)/include/pgscript/expressions/pgsGreaterEqual.h \
	$(srcdir)/include/pgscript/expressions/pgsIdent.h \
	$(srcdir)/include/pgscript/expressions/pgsIdentRecord.h \
	$(srcdir)/include/pgscript/expressions/pgsLines.h \
	$(srcdir)/include/pgscript/expressions/pgsLower.h \
	$(srcdir)/include/pgscript/expressions/pgsLowerEqual.h \
	$(srcdir)/include/pgscript/expressions/pgsMinus.h \
	$(srcdir)/include/pgscript/expressions/pgsModulo.h \
	$(srcdir)/include/pgscript/expressions/pgsNegate.h \
	$(srcdir)/include/pgscript/expressions/pgsNot.h \
	$(srcdir)/include/pgscript/expressions/pgsOperation.h \
	$(srcdir)/include/pgscript/expressions/pgsOr.h \
	$(srcdir)/include/pgscript/expressions/pgsOver.h \
	$(srcdir)/include/pgscript/expressions/pgsParenthesis.h \
	$(srcdir)/include/pgscript/expressions/pgsPlus.h \
	$(srcdir)/include/pgscript/expressions/pgsRemoveLine.h \
	$(srcdir)/include/pgscript/expressions/pgsTimes.h \
	$(srcdir)/include/pgscript/expressions/pgsTrim.h

EXTRA_DIST += \
	$(srcdir)/include/pgscript/expressions/module.mk

