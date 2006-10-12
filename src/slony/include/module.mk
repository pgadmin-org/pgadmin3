#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2006, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - src/slony/include/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/slony/include/dlgRepCluster.h \
	$(srcdir)/slony/include/dlgRepListen.h \
	$(srcdir)/slony/include/dlgRepNode.h \
	$(srcdir)/slony/include/dlgRepPath.h \
	$(srcdir)/slony/include/dlgRepProperty.h \
	$(srcdir)/slony/include/dlgRepSequence.h \
	$(srcdir)/slony/include/dlgRepSet.h \
	$(srcdir)/slony/include/dlgRepSubscription.h \
	$(srcdir)/slony/include/dlgRepTable.h \
	$(srcdir)/slony/include/slCluster.h \
	$(srcdir)/slony/include/slListen.h \
	$(srcdir)/slony/include/slNode.h \
	$(srcdir)/slony/include/slPath.h \
	$(srcdir)/slony/include/slSequence.h \
	$(srcdir)/slony/include/slSet.h \
	$(srcdir)/slony/include/slSubscription.h \
	$(srcdir)/slony/include/slTable.h

EXTRA_DIST += \
        $(srcdir)/slony/include/module.mk

