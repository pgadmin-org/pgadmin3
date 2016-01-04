#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dlg/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	dlg/dlgAddFavourite.cpp \
	dlg/dlgAggregate.cpp \
	dlg/dlgCast.cpp \
	dlg/dlgClasses.cpp \
	dlg/dlgCheck.cpp \
	dlg/dlgCollation.cpp \
	dlg/dlgColumn.cpp \
	dlg/dlgConnect.cpp \
	dlg/dlgConversion.cpp \
	dlg/dlgDatabase.cpp \
	dlg/dlgDomain.cpp \
	dlg/dlgEventTrigger.cpp \
	dlg/dlgExtension.cpp \
	dlg/dlgEditGridOptions.cpp \
	dlg/dlgFindReplace.cpp \
	dlg/dlgForeignDataWrapper.cpp \
	dlg/dlgForeignKey.cpp \
	dlg/dlgForeignServer.cpp \
	dlg/dlgForeignTable.cpp \
	dlg/dlgFunction.cpp \
	dlg/dlgGroup.cpp \
	dlg/dlgHbaConfig.cpp \
	dlg/dlgIndex.cpp \
	dlg/dlgIndexConstraint.cpp \
	dlg/dlgLanguage.cpp \
	dlg/dlgMainConfig.cpp \
	dlg/dlgManageFavourites.cpp \
	dlg/dlgMoveTablespace.cpp \
	dlg/dlgOperator.cpp \
	dlg/dlgPackage.cpp \
	dlg/dlgPgpassConfig.cpp \
	dlg/dlgProperty.cpp \
	dlg/dlgReassignDropOwned.cpp \
	dlg/dlgRole.cpp \
	dlg/dlgRule.cpp \
	dlg/dlgSchema.cpp \
	dlg/dlgSearchObject.cpp \
	dlg/dlgSelectConnection.cpp \
	dlg/dlgSequence.cpp \
	dlg/dlgServer.cpp \
	dlg/dlgSynonym.cpp \
	dlg/dlgTable.cpp \
	dlg/dlgTablespace.cpp \
	dlg/dlgTextSearchConfiguration.cpp \
	dlg/dlgTextSearchDictionary.cpp \
	dlg/dlgTextSearchParser.cpp \
	dlg/dlgTextSearchTemplate.cpp \
	dlg/dlgTrigger.cpp \
	dlg/dlgType.cpp \
	dlg/dlgUser.cpp \
	dlg/dlgUserMapping.cpp \
	dlg/dlgView.cpp \
	dlg/dlgManageMacros.cpp \
	dlg/dlgExtTable.cpp \
	dlg/dlgSelectDatabase.cpp \
	dlg/dlgResourceGroup.cpp

EXTRA_DIST += \
        dlg/module.mk 

