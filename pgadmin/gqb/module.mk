#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2008, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - pgadmin/gqb/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/gqb/gqbArrayCollection.cpp \
	$(srcdir)/gqb/gqbBrowser.cpp \
	$(srcdir)/gqb/gqbCollection.cpp \
	$(srcdir)/gqb/gqbColumn.cpp \
	$(srcdir)/gqb/gqbController.cpp \
	$(srcdir)/gqb/gqbDatabase.cpp \
	$(srcdir)/gqb/gqbGraphSimple.cpp \
	$(srcdir)/gqb/gqbGridOrderTable.cpp \
	$(srcdir)/gqb/gqbGridProjTable.cpp \
	$(srcdir)/gqb/gqbGridRestTable.cpp \
        $(srcdir)/gqb/gqbGridJoinTable.cpp \
	$(srcdir)/gqb/gqbModel.cpp \
	$(srcdir)/gqb/gqbObject.cpp \
	$(srcdir)/gqb/gqbObjectCollection.cpp \
	$(srcdir)/gqb/gqbQueryObjs.cpp \
	$(srcdir)/gqb/gqbSchema.cpp \
	$(srcdir)/gqb/gqbTable.cpp \
	$(srcdir)/gqb/gqbViewPanels.cpp \
	$(srcdir)/gqb/gqbView.cpp

EXTRA_DIST += \
    $(srcdir)/gqb/module.mk


