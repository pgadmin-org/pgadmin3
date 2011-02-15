#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/schema Makefile fragment
#
#######################################################################

subdir = $(srcdir)/schema

pgadmin3_SOURCES += \
        $(subdir)/edbPackage.cpp \
        $(subdir)/edbPackageFunction.cpp \
        $(subdir)/edbPackageVariable.cpp \
        $(subdir)/edbSynonym.cpp \
        $(subdir)/edbPrivateSynonym.cpp \
        $(subdir)/pgAggregate.cpp \
        $(subdir)/pgCast.cpp \
        $(subdir)/pgCatalogObject.cpp \
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
        $(subdir)/pgOperatorFamily.cpp \
        $(subdir)/pgRole.cpp \
        $(subdir)/pgRule.cpp \
        $(subdir)/pgSchema.cpp \
        $(subdir)/pgSequence.cpp \
        $(subdir)/pgServer.cpp \
        $(subdir)/pgTable.cpp \
        $(subdir)/pgTablespace.cpp \
        $(subdir)/pgTextSearchConfiguration.cpp \
        $(subdir)/pgTextSearchDictionary.cpp \
        $(subdir)/pgTextSearchParser.cpp \
        $(subdir)/pgTextSearchTemplate.cpp \
        $(subdir)/pgTrigger.cpp \
        $(subdir)/pgType.cpp \
        $(subdir)/pgUser.cpp \
        $(subdir)/pgView.cpp \
        $(subdir)/gpExtTable.cpp \
        $(subdir)/gpResQueue.cpp \
        $(subdir)/gpPartition.cpp

EXTRA_DIST += \
        $(srcdir)/schema/module.mk

