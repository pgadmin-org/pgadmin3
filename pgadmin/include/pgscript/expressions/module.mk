#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/pgscript/expressions/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	include/pgscript/expressions/pgsAnd.h \
	include/pgscript/expressions/pgsAssign.h \
	include/pgscript/expressions/pgsAssignToRecord.h \
	include/pgscript/expressions/pgsCast.h \
	include/pgscript/expressions/pgsColumns.h \
	include/pgscript/expressions/pgsDifferent.h \
	include/pgscript/expressions/pgsEqual.h \
	include/pgscript/expressions/pgsExecute.h \
	include/pgscript/expressions/pgsExpression.h \
	include/pgscript/expressions/pgsExpressions.h \
	include/pgscript/expressions/pgsGenDate.h \
	include/pgscript/expressions/pgsGenDateTime.h \
	include/pgscript/expressions/pgsGenDictionary.h \
	include/pgscript/expressions/pgsGenInt.h \
	include/pgscript/expressions/pgsGenReal.h \
	include/pgscript/expressions/pgsGenReference.h \
	include/pgscript/expressions/pgsGenRegex.h \
	include/pgscript/expressions/pgsGenString.h \
	include/pgscript/expressions/pgsGenTime.h \
	include/pgscript/expressions/pgsGreater.h \
	include/pgscript/expressions/pgsGreaterEqual.h \
	include/pgscript/expressions/pgsIdent.h \
	include/pgscript/expressions/pgsIdentRecord.h \
	include/pgscript/expressions/pgsLines.h \
	include/pgscript/expressions/pgsLower.h \
	include/pgscript/expressions/pgsLowerEqual.h \
	include/pgscript/expressions/pgsMinus.h \
	include/pgscript/expressions/pgsModulo.h \
	include/pgscript/expressions/pgsNegate.h \
	include/pgscript/expressions/pgsNot.h \
	include/pgscript/expressions/pgsOperation.h \
	include/pgscript/expressions/pgsOr.h \
	include/pgscript/expressions/pgsOver.h \
	include/pgscript/expressions/pgsParenthesis.h \
	include/pgscript/expressions/pgsPlus.h \
	include/pgscript/expressions/pgsRemoveLine.h \
	include/pgscript/expressions/pgsTimes.h \
	include/pgscript/expressions/pgsTrim.h

EXTRA_DIST += \
	include/pgscript/expressions/module.mk

