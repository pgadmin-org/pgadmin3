#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/ddmodel/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/dd/ddmodel/ddDatabaseDesign.cpp \
	$(srcdir)/dd/ddmodel/ddDrawingEditor.cpp \
	$(srcdir)/dd/ddmodel/ddDBReverseEnginering.cpp \
	$(srcdir)/dd/ddmodel/ddModelBrowser.cpp \
	$(srcdir)/dd/ddmodel/ddGenerationWizard.cpp \
	$(srcdir)/dd/ddmodel/ddDrawingView.cpp \
	$(srcdir)/dd/ddmodel/ddBrowserDataContainer.cpp

EXTRA_DIST += \
	$(srcdir)/dd/ddmodel/module.mk
