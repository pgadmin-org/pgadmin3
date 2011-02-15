#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/db/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	  $(srcdir)/include/db/pgConn.h \
	  $(srcdir)/include/db/pgQueryThread.h \
	  $(srcdir)/include/db/pgSet.h


EXTRA_DIST += \
    $(srcdir)/include/db/module.mk

