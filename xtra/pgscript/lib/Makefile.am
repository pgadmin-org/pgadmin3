##############################################################################
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2009, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# Makefile - Makefile for *nix systems
##############################################################################

noinst_LIBRARIES = libpgs.a

nodist_libpgs_a_SOURCES = $(srcdir)/../../../pgadmin/utils/sysLogger.cpp \
	$(srcdir)/../../../pgadmin/utils/misc.cpp \
	$(srcdir)/../../../pgadmin/db/keywords.c \
	$(srcdir)/../../../pgadmin/pgscript/utilities/pgsUtilities.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/pgsThread.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/pgsMapm.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/pgsDriver.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/pgsContext.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/pgsAlloc.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapm_sin.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapm_set.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapm_rnd.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapm_rcp.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapm_pow.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapm_mul.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapm_log.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapm_lg4.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapm_lg3.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapm_lg2.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapm_gcd.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapm_fpf.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapm_flr.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapm_fft.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapm_fam.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapm_exp.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapm_div.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapm_cpi.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapm_add.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapmutl2.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapmutl1.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapmutil.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapmstck.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapmsqrt.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapmrsin.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapmpwr2.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapmistr.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapmipwr.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapmhsin.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapmhasn.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapmgues.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapmfmul.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapmfact.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapmcnst.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapmcbrt.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapmasn0.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapmasin.cpp \
	$(srcdir)/../../../pgadmin/pgscript/utilities/m_apm/mapm5sin.cpp \
	$(srcdir)/../../../pgadmin/pgscript/statements/pgsWhileStmt.cpp \
	$(srcdir)/../../../pgadmin/pgscript/statements/pgsStmtList.cpp \
	$(srcdir)/../../../pgadmin/pgscript/statements/pgsStmt.cpp \
	$(srcdir)/../../../pgadmin/pgscript/statements/pgsProgram.cpp \
	$(srcdir)/../../../pgadmin/pgscript/statements/pgsPrintStmt.cpp \
	$(srcdir)/../../../pgadmin/pgscript/statements/pgsIfStmt.cpp \
	$(srcdir)/../../../pgadmin/pgscript/statements/pgsExpressionStmt.cpp \
	$(srcdir)/../../../pgadmin/pgscript/statements/pgsDeclareRecordStmt.cpp \
	$(srcdir)/../../../pgadmin/pgscript/statements/pgsContinueStmt.cpp \
	$(srcdir)/../../../pgadmin/pgscript/statements/pgsBreakStmt.cpp \
	$(srcdir)/../../../pgadmin/pgscript/statements/pgsAssertStmt.cpp \
	$(srcdir)/../../../pgadmin/pgscript/pgsApplication.cpp \
	$(srcdir)/../../../pgadmin/pgscript/parser.tab.cc \
	$(srcdir)/../../../pgadmin/pgscript/objects/pgsVariable.cpp \
	$(srcdir)/../../../pgadmin/pgscript/objects/pgsString.cpp \
	$(srcdir)/../../../pgadmin/pgscript/objects/pgsRecord.cpp \
	$(srcdir)/../../../pgadmin/pgscript/objects/pgsNumber.cpp \
	$(srcdir)/../../../pgadmin/pgscript/objects/pgsGenerator.cpp \
	$(srcdir)/../../../pgadmin/pgscript/lex.pgs.cc \
	$(srcdir)/../../../pgadmin/pgscript/generators/pgsTimeGen.cpp \
	$(srcdir)/../../../pgadmin/pgscript/generators/pgsStringGen.cpp \
	$(srcdir)/../../../pgadmin/pgscript/generators/pgsRegexGen.cpp \
	$(srcdir)/../../../pgadmin/pgscript/generators/pgsReferenceGen.cpp \
	$(srcdir)/../../../pgadmin/pgscript/generators/pgsRealGen.cpp \
	$(srcdir)/../../../pgadmin/pgscript/generators/pgsObjectGen.cpp \
	$(srcdir)/../../../pgadmin/pgscript/generators/pgsNumberGen.cpp \
	$(srcdir)/../../../pgadmin/pgscript/generators/pgsIntegerGen.cpp \
	$(srcdir)/../../../pgadmin/pgscript/generators/pgsDictionaryGen.cpp \
	$(srcdir)/../../../pgadmin/pgscript/generators/pgsDateTimeGen.cpp \
	$(srcdir)/../../../pgadmin/pgscript/generators/pgsDateGen.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsTrim.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsTimes.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsRemoveLine.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsPlus.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsParenthesis.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsOver.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsOr.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsOperation.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsNot.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsNegate.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsModulo.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsMinus.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsLowerEqual.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsLower.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsLines.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsIdentRecord.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsIdent.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsGreaterEqual.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsGreater.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsGenTime.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsGenString.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsGenRegex.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsGenReference.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsGenReal.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsGenInt.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsGenDictionary.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsGenDateTime.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsGenDate.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsExpression.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsExecute.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsEqual.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsDifferent.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsColumns.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsCast.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsAssignToRecord.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsAssign.cpp \
	$(srcdir)/../../../pgadmin/pgscript/expressions/pgsAnd.cpp \
	$(srcdir)/../../../pgadmin/pgscript/exceptions/pgsParameterException.cpp \
	$(srcdir)/../../../pgadmin/pgscript/exceptions/pgsInterruptException.cpp \
	$(srcdir)/../../../pgadmin/pgscript/exceptions/pgsException.cpp \
	$(srcdir)/../../../pgadmin/pgscript/exceptions/pgsContinueException.cpp \
	$(srcdir)/../../../pgadmin/pgscript/exceptions/pgsCastException.cpp \
	$(srcdir)/../../../pgadmin/pgscript/exceptions/pgsBreakException.cpp \
	$(srcdir)/../../../pgadmin/pgscript/exceptions/pgsAssertException.cpp \
	$(srcdir)/../../../pgadmin/pgscript/exceptions/pgsArithmeticException.cpp \
	$(srcdir)/../../../pgadmin/db/pgSet.cpp \
	$(srcdir)/../../../pgadmin/db/pgQueryThread.cpp \
	$(srcdir)/../../../pgadmin/db/pgConn.cpp

if SUN_CXX
  __CXXFLAGS=""
else
  __CXXFLAGS=-Wall -Wno-non-virtual-dtor -fno-strict-aliasing
endif

AM_CXXFLAGS = $(__CXXFLAGS) -I$(srcdir)/../../../pgadmin/include -DPGSCLI -DPGADMIN3_H
AM_CFLAGS = $(__CFLAGS) -I$(srcdir)/../../../pgadmin/include -DPGSCLI -DPGADMIN3_H
