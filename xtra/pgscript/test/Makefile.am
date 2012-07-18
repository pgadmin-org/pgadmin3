##############################################################################
# pgAdmin III - PostgreSQL Tools
#
# Copyright (C) 2002 - 2009, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# Makefile - Makefile for *nix systems
##############################################################################

noinst_PROGRAMS = pgsTest

pgsTest_SOURCES = \
	pgsTestUtilityUtilities.cpp \
	pgsTestUtilityMapm.cpp \
	pgsTestUtilityContext.cpp \
	pgsTestSuite.cpp \
	pgsTestStatements.cpp \
	pgsTestOperatorString.cpp \
	pgsTestOperatorRecord.cpp \
	pgsTestOperatorNumber.cpp \
	pgsTestOperatorGenerator.cpp \
	pgsTestObjectVariable.cpp \
	pgsTestObjectRecord.cpp \
	pgsTestObjectGenerator.cpp \
	pgsTestGeneratorTime.cpp \
	pgsTestGeneratorString.cpp \
	pgsTestGeneratorRegex.cpp \
	pgsTestGeneratorReference.cpp \
	pgsTestGeneratorReal.cpp \
	pgsTestGeneratorInt.cpp \
	pgsTestGeneratorDictionary.cpp \
	pgsTestGeneratorDateTime.cpp \
	pgsTestGeneratorDate.cpp \
	pgsTestExpressionRecord.cpp \
	pgsTestExpressionOperation.cpp \
	pgsTestExpressionIdent.cpp \
	pgsTestExpressionExecute.cpp \
	pgsTestExpressionCast.cpp

pgsTest_LDADD += \
	$(srcdir)/../lib/libpgs.a

noinst_HEADERS = \
	$(srcdir)/pgsTestSuite.h

if SUN_CXX
  __CXXFLAGS=""
else
  __CXXFLAGS=-Wall -Wno-non-virtual-dtor -fno-strict-aliasing
endif

AM_CXXFLAGS = $(__CXXFLAGS) -I$(srcdir)/../../../pgadmin/include -DPGSCLI -DPGADMIN3_H
AM_CFLAGS = $(__CFLAGS) -I$(srcdir)/../../../pgadmin/include -DPGSCLI -DPGADMIN3_H
