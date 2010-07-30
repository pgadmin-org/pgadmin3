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
	$(srcdir)/pgsTestUtilityUtilities.cpp \
	$(srcdir)/pgsTestUtilityMapm.cpp \
	$(srcdir)/pgsTestUtilityContext.cpp \
	$(srcdir)/pgsTestSuite.cpp \
	$(srcdir)/pgsTestStatements.cpp \
	$(srcdir)/pgsTestOperatorString.cpp \
	$(srcdir)/pgsTestOperatorRecord.cpp \
	$(srcdir)/pgsTestOperatorNumber.cpp \
	$(srcdir)/pgsTestOperatorGenerator.cpp \
	$(srcdir)/pgsTestObjectVariable.cpp \
	$(srcdir)/pgsTestObjectRecord.cpp \
	$(srcdir)/pgsTestObjectGenerator.cpp \
	$(srcdir)/pgsTestGeneratorTime.cpp \
	$(srcdir)/pgsTestGeneratorString.cpp \
	$(srcdir)/pgsTestGeneratorRegex.cpp \
	$(srcdir)/pgsTestGeneratorReference.cpp \
	$(srcdir)/pgsTestGeneratorReal.cpp \
	$(srcdir)/pgsTestGeneratorInt.cpp \
	$(srcdir)/pgsTestGeneratorDictionary.cpp \
	$(srcdir)/pgsTestGeneratorDateTime.cpp \
	$(srcdir)/pgsTestGeneratorDate.cpp \
	$(srcdir)/pgsTestExpressionRecord.cpp \
	$(srcdir)/pgsTestExpressionOperation.cpp \
	$(srcdir)/pgsTestExpressionIdent.cpp \
	$(srcdir)/pgsTestExpressionExecute.cpp \
	$(srcdir)/pgsTestExpressionCast.cpp

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
