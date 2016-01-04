#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/gqb/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	gqb/gqbArrayCollection.cpp \
	gqb/gqbBrowser.cpp \
	gqb/gqbCollection.cpp \
	gqb/gqbColumn.cpp \
	gqb/gqbController.cpp \
	gqb/gqbDatabase.cpp \
	gqb/gqbGraphSimple.cpp \
	gqb/gqbGridOrderTable.cpp \
	gqb/gqbGridProjTable.cpp \
	gqb/gqbGridRestTable.cpp \
        gqb/gqbGridJoinTable.cpp \
	gqb/gqbModel.cpp \
	gqb/gqbObject.cpp \
	gqb/gqbObjectCollection.cpp \
	gqb/gqbQueryObjs.cpp \
	gqb/gqbSchema.cpp \
	gqb/gqbTable.cpp \
	gqb/gqbViewPanels.cpp \
	gqb/gqbView.cpp

EXTRA_DIST += \
    gqb/module.mk


