#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/utild/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/utils/csvfiles.h \
	$(srcdir)/include/utils/factory.h \
	$(srcdir)/include/utils/favourites.h \
	$(srcdir)/include/utils/misc.h \
	$(srcdir)/include/utils/pgfeatures.h \
	$(srcdir)/include/utils/pgDefs.h \
	$(srcdir)/include/utils/pgconfig.h \
	$(srcdir)/include/utils/registry.h \
	$(srcdir)/include/utils/sysLogger.h \
	$(srcdir)/include/utils/sysProcess.h \
	$(srcdir)/include/utils/sysSettings.h \
	$(srcdir)/include/utils/utffile.h \
	$(srcdir)/include/utils/macros.h

EXTRA_DIST += \
        $(srcdir)/include/utils/module.mk

