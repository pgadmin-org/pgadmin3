#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2006, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - src/db/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/db/keywords.c \
	$(srcdir)/db/pgConn.cpp \
	$(srcdir)/db/pgSet.cpp

EXTRA_DIST += \
        $(srcdir)/db/module.mk

