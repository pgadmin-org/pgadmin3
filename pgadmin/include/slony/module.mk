#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2007, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - src/include/slony/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
    $(srcdir)/include/slony/dlgRepCluster.h \
    $(srcdir)/include/slony/dlgRepListen.h \
    $(srcdir)/include/slony/dlgRepNode.h \
    $(srcdir)/include/slony/dlgRepPath.h \
    $(srcdir)/include/slony/dlgRepProperty.h \
    $(srcdir)/include/slony/dlgRepSequence.h \
    $(srcdir)/include/slony/dlgRepSet.h \
    $(srcdir)/include/slony/dlgRepSubscription.h \
    $(srcdir)/include/slony/dlgRepTable.h \
    $(srcdir)/include/slony/slCluster.h \
    $(srcdir)/include/slony/slListen.h \
    $(srcdir)/include/slony/slNode.h \
    $(srcdir)/include/slony/slPath.h \
    $(srcdir)/include/slony/slSequence.h \
    $(srcdir)/include/slony/slSet.h \
    $(srcdir)/include/slony/slSubscription.h \
    $(srcdir)/include/slony/slTable.h

EXTRA_DIST += \
    $(srcdir)/include/slony/module.mk

