#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2009, The pgAdmin Development Team
# This software is released under the BSD Licence
#
# module.mk - pgadmin/include/gqb/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
    $(srcdir)/include/gqb/gqbArrayCollection.h \
    $(srcdir)/include/gqb/gqbBrowser.h \
    $(srcdir)/include/gqb/gqbCollection.h \
    $(srcdir)/include/gqb/gqbCollectionBase.h \
    $(srcdir)/include/gqb/gqbColumn.h \
    $(srcdir)/include/gqb/gqbDatabase.h \
    $(srcdir)/include/gqb/gqbEvents.h \
    $(srcdir)/include/gqb/gqbGraphBehavior.h \
    $(srcdir)/include/gqb/gqbGraphSimple.h \
    $(srcdir)/include/gqb/gqbGridOrderTable.h \
    $(srcdir)/include/gqb/gqbGridProjTable.h \
    $(srcdir)/include/gqb/gqbGridRestTable.h \
    $(srcdir)/include/gqb/gqbGridJoinTable.h \
    $(srcdir)/include/gqb/gqbModel.h \
    $(srcdir)/include/gqb/gqbObject.h \
    $(srcdir)/include/gqb/gqbObjectCollection.h \
    $(srcdir)/include/gqb/gqbQueryObjs.h \
    $(srcdir)/include/gqb/gqbSchema.h \
    $(srcdir)/include/gqb/gqbTable.h \
    $(srcdir)/include/gqb/gqbViewController.h \
    $(srcdir)/include/gqb/gqbViewPanels.h

EXTRA_DIST += \
    $(srcdir)/include/gqb/module.mk

