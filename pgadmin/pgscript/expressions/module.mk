#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/pgscript/expressions/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	pgscript/expressions/pgsAnd.cpp \
	pgscript/expressions/pgsAssign.cpp \
	pgscript/expressions/pgsAssignToRecord.cpp \
	pgscript/expressions/pgsCast.cpp \
	pgscript/expressions/pgsColumns.cpp \
	pgscript/expressions/pgsDifferent.cpp \
	pgscript/expressions/pgsEqual.cpp \
	pgscript/expressions/pgsExecute.cpp \
	pgscript/expressions/pgsExpression.cpp \
	pgscript/expressions/pgsGenDate.cpp \
	pgscript/expressions/pgsGenDateTime.cpp \
	pgscript/expressions/pgsGenDictionary.cpp \
	pgscript/expressions/pgsGenInt.cpp \
	pgscript/expressions/pgsGenReal.cpp \
	pgscript/expressions/pgsGenReference.cpp \
	pgscript/expressions/pgsGenRegex.cpp \
	pgscript/expressions/pgsGenString.cpp \
	pgscript/expressions/pgsGenTime.cpp \
	pgscript/expressions/pgsGreater.cpp \
	pgscript/expressions/pgsGreaterEqual.cpp \
	pgscript/expressions/pgsIdent.cpp \
	pgscript/expressions/pgsIdentRecord.cpp \
	pgscript/expressions/pgsLines.cpp \
	pgscript/expressions/pgsLower.cpp \
	pgscript/expressions/pgsLowerEqual.cpp \
	pgscript/expressions/pgsMinus.cpp \
	pgscript/expressions/pgsModulo.cpp \
	pgscript/expressions/pgsNegate.cpp \
	pgscript/expressions/pgsNot.cpp \
	pgscript/expressions/pgsOperation.cpp \
	pgscript/expressions/pgsOr.cpp \
	pgscript/expressions/pgsOver.cpp \
	pgscript/expressions/pgsParenthesis.cpp \
	pgscript/expressions/pgsPlus.cpp \
	pgscript/expressions/pgsRemoveLine.cpp \
	pgscript/expressions/pgsTimes.cpp \
	pgscript/expressions/pgsTrim.cpp

EXTRA_DIST += \
	pgscript/expressions/module.mk

