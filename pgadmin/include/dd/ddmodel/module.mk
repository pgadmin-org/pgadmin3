#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/dd/ddmodel/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	include/dd/ddmodel/ddBrowserDataContainer.h \
	include/dd/ddmodel/ddDatabaseDesign.h \
	include/dd/ddmodel/ddDrawingEditor.h \
	include/dd/ddmodel/ddDBReverseEngineering.h \
	include/dd/ddmodel/ddDrawingView.h \
	include/dd/ddmodel/ddGenerationWizard.h \
	include/dd/ddmodel/ddModelBrowser.h
	
EXTRA_DIST += \
	include/dd/ddmodel/module.mk
