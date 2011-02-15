#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/pgscript/utilities/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/pgscript/utilities/pgsAlloc.h \
	$(srcdir)/include/pgscript/utilities/pgsContext.h \
	$(srcdir)/include/pgscript/utilities/pgsCopiedPtr.h \
	$(srcdir)/include/pgscript/utilities/pgsDriver.h \
	$(srcdir)/include/pgscript/utilities/pgsMapm.h \
	$(srcdir)/include/pgscript/utilities/pgsScanner.h \
	$(srcdir)/include/pgscript/utilities/pgsSharedPtr.h \
	$(srcdir)/include/pgscript/utilities/pgsThread.h \
	$(srcdir)/include/pgscript/utilities/pgsUtilities.h

EXTRA_DIST += \
	$(srcdir)/include/pgscript/utilities/module.mk

include $(srcdir)/include/pgscript/utilities/mapm-lib/module.mk
