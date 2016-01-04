#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/ddmodel/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	dd/ddmodel/ddDatabaseDesign.cpp \
	dd/ddmodel/ddDrawingEditor.cpp \
	dd/ddmodel/ddDBReverseEnginering.cpp \
	dd/ddmodel/ddModelBrowser.cpp \
	dd/ddmodel/ddGenerationWizard.cpp \
	dd/ddmodel/ddDrawingView.cpp \
	dd/ddmodel/ddBrowserDataContainer.cpp

EXTRA_DIST += \
	dd/ddmodel/module.mk
