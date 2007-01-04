#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2007, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - src/schema Makefile fragment
#
#######################################################################

subdir = $(srcdir)/schema

pgadmin3_SOURCES += \
	$(subdir)/pgAggregate.cpp \
	$(subdir)/pgCast.cpp \
	$(subdir)/pgCheck.cpp \
        $(subdir)/pgCollection.cpp \
        $(subdir)/pgColumn.cpp \
        $(subdir)/pgConstraints.cpp \
        $(subdir)/pgConversion.cpp \
        $(subdir)/pgDatabase.cpp \
        $(subdir)/pgDatatype.cpp \
        $(subdir)/pgDomain.cpp \
        $(subdir)/pgForeignKey.cpp \
        $(subdir)/pgFunction.cpp \
        $(subdir)/pgGroup.cpp \
        $(subdir)/pgIndex.cpp \
        $(subdir)/pgIndexConstraint.cpp \
        $(subdir)/pgLanguage.cpp \
        $(subdir)/pgObject.cpp \
        $(subdir)/pgOperator.cpp \
        $(subdir)/pgOperatorClass.cpp \
        $(subdir)/pgRole.cpp \
        $(subdir)/pgRule.cpp \
        $(subdir)/pgSchema.cpp \
        $(subdir)/pgSequence.cpp \
        $(subdir)/pgServer.cpp \
        $(subdir)/pgTable.cpp \
        $(subdir)/pgTablespace.cpp \
        $(subdir)/pgTrigger.cpp \
        $(subdir)/pgType.cpp \
        $(subdir)/pgUser.cpp \
        $(subdir)/pgView.cpp

EXTRA_DIST += \
        $(srcdir)/schema/module.mk

