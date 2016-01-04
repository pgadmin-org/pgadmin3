#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/gqb/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
    include/gqb/gqbArrayCollection.h \
    include/gqb/gqbBrowser.h \
    include/gqb/gqbCollection.h \
    include/gqb/gqbCollectionBase.h \
    include/gqb/gqbColumn.h \
    include/gqb/gqbDatabase.h \
    include/gqb/gqbEvents.h \
    include/gqb/gqbGraphBehavior.h \
    include/gqb/gqbGraphSimple.h \
    include/gqb/gqbGridOrderTable.h \
    include/gqb/gqbGridProjTable.h \
    include/gqb/gqbGridRestTable.h \
    include/gqb/gqbGridJoinTable.h \
    include/gqb/gqbModel.h \
    include/gqb/gqbObject.h \
    include/gqb/gqbObjectCollection.h \
    include/gqb/gqbQueryObjs.h \
    include/gqb/gqbSchema.h \
    include/gqb/gqbTable.h \
    include/gqb/gqbViewController.h \
    include/gqb/gqbViewPanels.h

EXTRA_DIST += \
    include/gqb/module.mk

