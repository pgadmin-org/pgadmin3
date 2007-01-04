#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: module.mk 5466 2006-10-12 09:31:39Z dpage $
# Copyright (C) 2002 - 2007, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - src/include/db/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	  $(srcdir)/include/db/pgConn.h \
	  $(srcdir)/include/db/pgQueryThread.h \
	  $(srcdir)/include/db/pgSet.h


EXTRA_DIST += \
    $(srcdir)/include/db/module.mk

