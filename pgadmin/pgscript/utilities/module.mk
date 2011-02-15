#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/pgscript/utilities/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/pgscript/utilities/pgsAlloc.cpp \
	$(srcdir)/pgscript/utilities/pgsContext.cpp \
	$(srcdir)/pgscript/utilities/pgsDriver.cpp \
	$(srcdir)/pgscript/utilities/pgsMapm.cpp \
	$(srcdir)/pgscript/utilities/pgsThread.cpp \
	$(srcdir)/pgscript/utilities/pgsUtilities.cpp

EXTRA_DIST += \
	$(srcdir)/pgscript/utilities/module.mk

include $(srcdir)/pgscript/utilities/m_apm/module.mk
