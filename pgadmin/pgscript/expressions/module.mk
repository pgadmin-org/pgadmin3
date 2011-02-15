#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/pgscript/expressions/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/pgscript/expressions/pgsAnd.cpp \
	$(srcdir)/pgscript/expressions/pgsAssign.cpp \
	$(srcdir)/pgscript/expressions/pgsAssignToRecord.cpp \
	$(srcdir)/pgscript/expressions/pgsCast.cpp \
	$(srcdir)/pgscript/expressions/pgsColumns.cpp \
	$(srcdir)/pgscript/expressions/pgsDifferent.cpp \
	$(srcdir)/pgscript/expressions/pgsEqual.cpp \
	$(srcdir)/pgscript/expressions/pgsExecute.cpp \
	$(srcdir)/pgscript/expressions/pgsExpression.cpp \
	$(srcdir)/pgscript/expressions/pgsGenDate.cpp \
	$(srcdir)/pgscript/expressions/pgsGenDateTime.cpp \
	$(srcdir)/pgscript/expressions/pgsGenDictionary.cpp \
	$(srcdir)/pgscript/expressions/pgsGenInt.cpp \
	$(srcdir)/pgscript/expressions/pgsGenReal.cpp \
	$(srcdir)/pgscript/expressions/pgsGenReference.cpp \
	$(srcdir)/pgscript/expressions/pgsGenRegex.cpp \
	$(srcdir)/pgscript/expressions/pgsGenString.cpp \
	$(srcdir)/pgscript/expressions/pgsGenTime.cpp \
	$(srcdir)/pgscript/expressions/pgsGreater.cpp \
	$(srcdir)/pgscript/expressions/pgsGreaterEqual.cpp \
	$(srcdir)/pgscript/expressions/pgsIdent.cpp \
	$(srcdir)/pgscript/expressions/pgsIdentRecord.cpp \
	$(srcdir)/pgscript/expressions/pgsLines.cpp \
	$(srcdir)/pgscript/expressions/pgsLower.cpp \
	$(srcdir)/pgscript/expressions/pgsLowerEqual.cpp \
	$(srcdir)/pgscript/expressions/pgsMinus.cpp \
	$(srcdir)/pgscript/expressions/pgsModulo.cpp \
	$(srcdir)/pgscript/expressions/pgsNegate.cpp \
	$(srcdir)/pgscript/expressions/pgsNot.cpp \
	$(srcdir)/pgscript/expressions/pgsOperation.cpp \
	$(srcdir)/pgscript/expressions/pgsOr.cpp \
	$(srcdir)/pgscript/expressions/pgsOver.cpp \
	$(srcdir)/pgscript/expressions/pgsParenthesis.cpp \
	$(srcdir)/pgscript/expressions/pgsPlus.cpp \
	$(srcdir)/pgscript/expressions/pgsRemoveLine.cpp \
	$(srcdir)/pgscript/expressions/pgsTimes.cpp \
	$(srcdir)/pgscript/expressions/pgsTrim.cpp

EXTRA_DIST += \
	$(srcdir)/pgscript/expressions/module.mk

