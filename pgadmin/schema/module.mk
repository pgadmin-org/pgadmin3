#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/schema Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
        schema/edbPackage.cpp \
        schema/edbPackageFunction.cpp \
        schema/edbPackageVariable.cpp \
        schema/edbSynonym.cpp \
        schema/edbPrivateSynonym.cpp \
        schema/pgAggregate.cpp \
        schema/pgCast.cpp \
        schema/pgCatalogObject.cpp \
        schema/pgCheck.cpp \
        schema/pgCollation.cpp \
        schema/pgCollection.cpp \
        schema/pgColumn.cpp \
        schema/pgConstraints.cpp \
        schema/pgConversion.cpp \
        schema/pgDatabase.cpp \
        schema/pgDatatype.cpp \
        schema/pgDomain.cpp \
        schema/pgEventTrigger.cpp \
        schema/pgExtension.cpp \
        schema/pgForeignDataWrapper.cpp \
        schema/pgForeignKey.cpp \
        schema/pgForeignServer.cpp \
        schema/pgForeignTable.cpp \
        schema/pgFunction.cpp \
        schema/pgGroup.cpp \
        schema/pgIndex.cpp \
        schema/pgIndexConstraint.cpp \
        schema/pgLanguage.cpp \
        schema/pgObject.cpp \
        schema/pgOperator.cpp \
        schema/pgOperatorClass.cpp \
        schema/pgOperatorFamily.cpp \
        schema/pgRole.cpp \
        schema/pgRule.cpp \
        schema/pgSchema.cpp \
        schema/pgSequence.cpp \
        schema/pgServer.cpp \
        schema/pgTable.cpp \
        schema/pgTablespace.cpp \
        schema/pgTextSearchConfiguration.cpp \
        schema/pgTextSearchDictionary.cpp \
        schema/pgTextSearchParser.cpp \
        schema/pgTextSearchTemplate.cpp \
        schema/pgTrigger.cpp \
        schema/pgType.cpp \
        schema/pgUser.cpp \
        schema/pgUserMapping.cpp \
        schema/pgView.cpp \
        schema/gpExtTable.cpp \
        schema/gpResQueue.cpp \
        schema/gpPartition.cpp \
        schema/edbResourceGroup.cpp

EXTRA_DIST += \
        schema/module.mk

