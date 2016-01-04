#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/slony/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
    include/slony/dlgRepCluster.h \
    include/slony/dlgRepListen.h \
    include/slony/dlgRepNode.h \
    include/slony/dlgRepPath.h \
    include/slony/dlgRepProperty.h \
    include/slony/dlgRepSequence.h \
    include/slony/dlgRepSet.h \
    include/slony/dlgRepSubscription.h \
    include/slony/dlgRepTable.h \
    include/slony/slCluster.h \
    include/slony/slListen.h \
    include/slony/slNode.h \
    include/slony/slPath.h \
    include/slony/slSequence.h \
    include/slony/slSet.h \
    include/slony/slSubscription.h \
    include/slony/slTable.h

EXTRA_DIST += \
    include/slony/module.mk

