#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/db/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/db/keywords.c \
	$(srcdir)/db/pgConn.cpp \
	$(srcdir)/db/pgSet.cpp \
	$(srcdir)/db/pgQueryThread.cpp

EXTRA_DIST += \
        $(srcdir)/db/module.mk

