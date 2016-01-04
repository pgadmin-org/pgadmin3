#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/dlg/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	include/dlg/dlgAddFavourite.h \
	include/dlg/dlgAggregate.h \
	include/dlg/dlgCast.h \
	include/dlg/dlgCheck.h \
	include/dlg/dlgClasses.h \
	include/dlg/dlgCollation.h \
	include/dlg/dlgColumn.h \
	include/dlg/dlgConnect.h \
	include/dlg/dlgConversion.h \
	include/dlg/dlgDatabase.h \
	include/dlg/dlgDomain.h \
	include/dlg/dlgEventTrigger.h \
	include/dlg/dlgExtension.h \
	include/dlg/dlgEditGridOptions.h \
	include/dlg/dlgFindReplace.h \
	include/dlg/dlgForeignDataWrapper.h \
	include/dlg/dlgForeignKey.h \
	include/dlg/dlgForeignServer.h \
	include/dlg/dlgForeignTable.h \
	include/dlg/dlgFunction.h \
	include/dlg/dlgGroup.h \
	include/dlg/dlgHbaConfig.h \
	include/dlg/dlgIndex.h \
	include/dlg/dlgIndexConstraint.h \
	include/dlg/dlgLanguage.h \
	include/dlg/dlgMainConfig.h \
	include/dlg/dlgManageFavourites.h \
	include/dlg/dlgMoveTablespace.h \
	include/dlg/dlgOperator.h \
	include/dlg/dlgPackage.h \
	include/dlg/dlgPgpassConfig.h \
	include/dlg/dlgProperty.h \
	include/dlg/dlgReassignDropOwned.h \
	include/dlg/dlgRole.h \
	include/dlg/dlgRule.h \
	include/dlg/dlgSchema.h \
	include/dlg/dlgSearchObject.h \
	include/dlg/dlgSelectConnection.h \
	include/dlg/dlgSequence.h \
	include/dlg/dlgServer.h \
	include/dlg/dlgSynonym.h \
	include/dlg/dlgTable.h \
	include/dlg/dlgTablespace.h \
	include/dlg/dlgTextSearchConfiguration.h \
	include/dlg/dlgTextSearchDictionary.h \
	include/dlg/dlgTextSearchParser.h \
	include/dlg/dlgTextSearchTemplate.h \
	include/dlg/dlgTrigger.h \
	include/dlg/dlgType.h \
	include/dlg/dlgUser.h \
	include/dlg/dlgUserMapping.h \
	include/dlg/dlgView.h \
	include/dlg/dlgManageMacros.h \
	include/dlg/dlgExtTable.h \
	include/dlg/dlgSelectDatabase.h \
	include/dlg/dlgResourceGroup.h

EXTRA_DIST += \
        include/dlg/module.mk

