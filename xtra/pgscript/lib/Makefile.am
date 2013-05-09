##############################################################################
# pgAdmin III - PostgreSQL Tools
#
# Copyright (C) 2002 - 2009, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# Makefile - Makefile for *nix systems
##############################################################################

AUTOMAKE_OPTIONS = foreign subdir-objects no-dependencies
OBJEXT=pgso
noinst_LIBRARIES = libpgs.a

libpgs_a_SOURCES = ../../../pgadmin/utils/sysLogger.cpp \
	../../../pgadmin/utils/misc.cpp \
	../../../pgadmin/db/keywords.c \
	../../../pgadmin/pgscript/utilities/pgsUtilities.cpp \
	../../../pgadmin/pgscript/utilities/pgsThread.cpp \
	../../../pgadmin/pgscript/utilities/pgsMapm.cpp \
	../../../pgadmin/pgscript/utilities/pgsDriver.cpp \
	../../../pgadmin/pgscript/utilities/pgsContext.cpp \
	../../../pgadmin/pgscript/utilities/pgsAlloc.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapm_sin.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapm_set.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapm_rnd.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapm_rcp.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapm_pow.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapm_mul.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapm_log.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapm_lg4.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapm_lg3.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapm_lg2.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapm_gcd.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapm_fpf.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapm_flr.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapm_fft.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapm_fam.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapm_exp.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapm_div.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapm_cpi.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapm_add.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapmutl2.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapmutl1.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapmutil.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapmstck.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapmsqrt.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapmrsin.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapmpwr2.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapmistr.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapmipwr.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapmhsin.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapmhasn.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapmgues.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapmfmul.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapmfact.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapmcnst.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapmcbrt.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapmasn0.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapmasin.cpp \
	../../../pgadmin/pgscript/utilities/m_apm/mapm5sin.cpp \
	../../../pgadmin/pgscript/statements/pgsWhileStmt.cpp \
	../../../pgadmin/pgscript/statements/pgsStmtList.cpp \
	../../../pgadmin/pgscript/statements/pgsStmt.cpp \
	../../../pgadmin/pgscript/statements/pgsProgram.cpp \
	../../../pgadmin/pgscript/statements/pgsPrintStmt.cpp \
	../../../pgadmin/pgscript/statements/pgsIfStmt.cpp \
	../../../pgadmin/pgscript/statements/pgsExpressionStmt.cpp \
	../../../pgadmin/pgscript/statements/pgsDeclareRecordStmt.cpp \
	../../../pgadmin/pgscript/statements/pgsContinueStmt.cpp \
	../../../pgadmin/pgscript/statements/pgsBreakStmt.cpp \
	../../../pgadmin/pgscript/statements/pgsAssertStmt.cpp \
	../../../pgadmin/pgscript/pgsApplication.cpp \
	../../../pgadmin/pgscript/parser.tab.cc \
	../../../pgadmin/pgscript/objects/pgsVariable.cpp \
	../../../pgadmin/pgscript/objects/pgsString.cpp \
	../../../pgadmin/pgscript/objects/pgsRecord.cpp \
	../../../pgadmin/pgscript/objects/pgsNumber.cpp \
	../../../pgadmin/pgscript/objects/pgsGenerator.cpp \
	../../../pgadmin/pgscript/lex.pgs.cc \
	../../../pgadmin/pgscript/generators/pgsTimeGen.cpp \
	../../../pgadmin/pgscript/generators/pgsStringGen.cpp \
	../../../pgadmin/pgscript/generators/pgsRegexGen.cpp \
	../../../pgadmin/pgscript/generators/pgsReferenceGen.cpp \
	../../../pgadmin/pgscript/generators/pgsRealGen.cpp \
	../../../pgadmin/pgscript/generators/pgsObjectGen.cpp \
	../../../pgadmin/pgscript/generators/pgsNumberGen.cpp \
	../../../pgadmin/pgscript/generators/pgsIntegerGen.cpp \
	../../../pgadmin/pgscript/generators/pgsDictionaryGen.cpp \
	../../../pgadmin/pgscript/generators/pgsDateTimeGen.cpp \
	../../../pgadmin/pgscript/generators/pgsDateGen.cpp \
	../../../pgadmin/pgscript/expressions/pgsTrim.cpp \
	../../../pgadmin/pgscript/expressions/pgsTimes.cpp \
	../../../pgadmin/pgscript/expressions/pgsRemoveLine.cpp \
	../../../pgadmin/pgscript/expressions/pgsPlus.cpp \
	../../../pgadmin/pgscript/expressions/pgsParenthesis.cpp \
	../../../pgadmin/pgscript/expressions/pgsOver.cpp \
	../../../pgadmin/pgscript/expressions/pgsOr.cpp \
	../../../pgadmin/pgscript/expressions/pgsOperation.cpp \
	../../../pgadmin/pgscript/expressions/pgsNot.cpp \
	../../../pgadmin/pgscript/expressions/pgsNegate.cpp \
	../../../pgadmin/pgscript/expressions/pgsModulo.cpp \
	../../../pgadmin/pgscript/expressions/pgsMinus.cpp \
	../../../pgadmin/pgscript/expressions/pgsLowerEqual.cpp \
	../../../pgadmin/pgscript/expressions/pgsLower.cpp \
	../../../pgadmin/pgscript/expressions/pgsLines.cpp \
	../../../pgadmin/pgscript/expressions/pgsIdentRecord.cpp \
	../../../pgadmin/pgscript/expressions/pgsIdent.cpp \
	../../../pgadmin/pgscript/expressions/pgsGreaterEqual.cpp \
	../../../pgadmin/pgscript/expressions/pgsGreater.cpp \
	../../../pgadmin/pgscript/expressions/pgsGenTime.cpp \
	../../../pgadmin/pgscript/expressions/pgsGenString.cpp \
	../../../pgadmin/pgscript/expressions/pgsGenRegex.cpp \
	../../../pgadmin/pgscript/expressions/pgsGenReference.cpp \
	../../../pgadmin/pgscript/expressions/pgsGenReal.cpp \
	../../../pgadmin/pgscript/expressions/pgsGenInt.cpp \
	../../../pgadmin/pgscript/expressions/pgsGenDictionary.cpp \
	../../../pgadmin/pgscript/expressions/pgsGenDateTime.cpp \
	../../../pgadmin/pgscript/expressions/pgsGenDate.cpp \
	../../../pgadmin/pgscript/expressions/pgsExpression.cpp \
	../../../pgadmin/pgscript/expressions/pgsExecute.cpp \
	../../../pgadmin/pgscript/expressions/pgsEqual.cpp \
	../../../pgadmin/pgscript/expressions/pgsDifferent.cpp \
	../../../pgadmin/pgscript/expressions/pgsColumns.cpp \
	../../../pgadmin/pgscript/expressions/pgsCast.cpp \
	../../../pgadmin/pgscript/expressions/pgsAssignToRecord.cpp \
	../../../pgadmin/pgscript/expressions/pgsAssign.cpp \
	../../../pgadmin/pgscript/expressions/pgsAnd.cpp \
	../../../pgadmin/pgscript/exceptions/pgsParameterException.cpp \
	../../../pgadmin/pgscript/exceptions/pgsInterruptException.cpp \
	../../../pgadmin/pgscript/exceptions/pgsException.cpp \
	../../../pgadmin/pgscript/exceptions/pgsContinueException.cpp \
	../../../pgadmin/pgscript/exceptions/pgsCastException.cpp \
	../../../pgadmin/pgscript/exceptions/pgsBreakException.cpp \
	../../../pgadmin/pgscript/exceptions/pgsAssertException.cpp \
	../../../pgadmin/pgscript/exceptions/pgsArithmeticException.cpp \
	../../../pgadmin/db/pgSet.cpp \
	../../../pgadmin/db/pgQueryThread.cpp \
	../../../pgadmin/db/pgConn.cpp

if SUN_CXX
  __CXXFLAGS=""
else
  __CXXFLAGS=-Wall -Wno-non-virtual-dtor -fno-strict-aliasing
endif

AM_CXXFLAGS = $(__CXXFLAGS) -I$(top_srcdir)/pgadmin/include -DPGSCLI -DPGADMIN3_H
AM_CFLAGS = $(__CFLAGS) -I$(top_srcdir)/pgadmin/include -DPGSCLI -DPGADMIN3_H

%.pgso : %.c
	$(CXXCOMPILE) -c -o $@ $<

%.pgso : %.cpp
	$(CXXCOMPILE) -c -o $@ $<
