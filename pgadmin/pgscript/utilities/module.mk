#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/pgscript/utilities/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	pgscript/utilities/pgsAlloc.cpp \
	pgscript/utilities/pgsContext.cpp \
	pgscript/utilities/pgsDriver.cpp \
	pgscript/utilities/pgsMapm.cpp \
	pgscript/utilities/pgsThread.cpp \
	pgscript/utilities/pgsUtilities.cpp

EXTRA_DIST += \
	pgscript/utilities/module.mk

include pgscript/utilities/m_apm/module.mk
