#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/schema/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/schema/edbPackage.h \
	$(srcdir)/include/schema/edbPackageFunction.h \
	$(srcdir)/include/schema/edbPackageVariable.h \
	$(srcdir)/include/schema/edbSynonym.h \
	$(srcdir)/include/schema/edbPrivateSynonym.h \
	$(srcdir)/include/schema/pgAggregate.h \
	$(srcdir)/include/schema/pgCatalogObject.h \
	$(srcdir)/include/schema/pgCast.h \
	$(srcdir)/include/schema/pgCheck.h \
	$(srcdir)/include/schema/pgCollation.h \
	$(srcdir)/include/schema/pgCollection.h \
	$(srcdir)/include/schema/pgColumn.h \
	$(srcdir)/include/schema/pgConstraints.h \
	$(srcdir)/include/schema/pgConversion.h \
	$(srcdir)/include/schema/pgDatabase.h \
	$(srcdir)/include/schema/pgDatatype.h \
	$(srcdir)/include/schema/pgDomain.h \
	$(srcdir)/include/schema/pgExtension.h \
	$(srcdir)/include/schema/pgForeignDataWrapper.h \
	$(srcdir)/include/schema/pgForeignKey.h \
	$(srcdir)/include/schema/pgForeignServer.h \
	$(srcdir)/include/schema/pgForeignTable.h \
	$(srcdir)/include/schema/pgFunction.h \
	$(srcdir)/include/schema/pgGroup.h \
	$(srcdir)/include/schema/pgIndex.h \
	$(srcdir)/include/schema/pgIndexConstraint.h \
	$(srcdir)/include/schema/pgLanguage.h \
	$(srcdir)/include/schema/pgObject.h \
	$(srcdir)/include/schema/pgOperator.h \
	$(srcdir)/include/schema/pgOperatorClass.h \
	$(srcdir)/include/schema/pgOperatorFamily.h \
	$(srcdir)/include/schema/pgRole.h \
	$(srcdir)/include/schema/pgRule.h \
	$(srcdir)/include/schema/pgSchema.h \
	$(srcdir)/include/schema/pgSequence.h \
	$(srcdir)/include/schema/pgServer.h \
	$(srcdir)/include/schema/pgTable.h \
	$(srcdir)/include/schema/pgTablespace.h \
  	$(srcdir)/include/schema/pgTextSearchConfiguration.h \
  	$(srcdir)/include/schema/pgTextSearchDictionary.h \
  	$(srcdir)/include/schema/pgTextSearchParser.h \
  	$(srcdir)/include/schema/pgTextSearchTemplate.h \
	$(srcdir)/include/schema/pgTrigger.h \
	$(srcdir)/include/schema/pgType.h \
	$(srcdir)/include/schema/pgUser.h \
	$(srcdir)/include/schema/pgUserMapping.h \
	$(srcdir)/include/schema/pgView.h \
	$(srcdir)/include/schema/gpExtTable.h \
	$(srcdir)/include/schema/gpResQueue.h \
	$(srcdir)/include/schema/gpPartition.h

EXTRA_DIST += \
	$(srcdir)/include/schema/module.mk

