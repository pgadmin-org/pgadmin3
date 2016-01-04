#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/db/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	  include/db/pgConn.h \
	  include/db/pgQueryThread.h \
	  include/db/pgQueryResultEvent.h \
	  include/db/pgSet.h

EXTRA_DIST += \
    include/db/module.mk

