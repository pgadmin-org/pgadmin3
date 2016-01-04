#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/schema/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	include/schema/edbPackage.h \
	include/schema/edbPackageFunction.h \
	include/schema/edbPackageVariable.h \
	include/schema/edbSynonym.h \
	include/schema/edbPrivateSynonym.h \
	include/schema/pgAggregate.h \
	include/schema/pgCatalogObject.h \
	include/schema/pgCast.h \
	include/schema/pgCheck.h \
	include/schema/pgCollation.h \
	include/schema/pgCollection.h \
	include/schema/pgColumn.h \
	include/schema/pgConstraints.h \
	include/schema/pgConversion.h \
	include/schema/pgDatabase.h \
	include/schema/pgDatatype.h \
	include/schema/pgDomain.h \
	include/schema/pgEventTrigger.h \
	include/schema/pgExtension.h \
	include/schema/pgForeignDataWrapper.h \
	include/schema/pgForeignKey.h \
	include/schema/pgForeignServer.h \
	include/schema/pgForeignTable.h \
	include/schema/pgFunction.h \
	include/schema/pgGroup.h \
	include/schema/pgIndex.h \
	include/schema/pgIndexConstraint.h \
	include/schema/pgLanguage.h \
	include/schema/pgObject.h \
	include/schema/pgOperator.h \
	include/schema/pgOperatorClass.h \
	include/schema/pgOperatorFamily.h \
	include/schema/pgRole.h \
	include/schema/pgRule.h \
	include/schema/pgSchema.h \
	include/schema/pgSequence.h \
	include/schema/pgServer.h \
	include/schema/pgTable.h \
	include/schema/pgTablespace.h \
  	include/schema/pgTextSearchConfiguration.h \
  	include/schema/pgTextSearchDictionary.h \
  	include/schema/pgTextSearchParser.h \
  	include/schema/pgTextSearchTemplate.h \
	include/schema/pgTrigger.h \
	include/schema/pgType.h \
	include/schema/pgUser.h \
	include/schema/pgUserMapping.h \
	include/schema/pgView.h \
	include/schema/gpExtTable.h \
	include/schema/gpResQueue.h \
	include/schema/gpPartition.h \
	include/schema/edbResourceGroup.h

EXTRA_DIST += \
	include/schema/module.mk

