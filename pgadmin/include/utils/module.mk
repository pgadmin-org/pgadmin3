#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2007, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - pgadmin/include/utild/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/utils/factory.h \
	$(srcdir)/include/utils/favourites.h \
	$(srcdir)/include/utils/md5.h \
	$(srcdir)/include/utils/misc.h \
	$(srcdir)/include/utils/pgfeatures.h \
	$(srcdir)/include/utils/pgDefs.h \
	$(srcdir)/include/utils/pgconfig.h \
	$(srcdir)/include/utils/sysLogger.h \
	$(srcdir)/include/utils/sysProcess.h \
	$(srcdir)/include/utils/sysSettings.h \
	$(srcdir)/include/utils/utffile.h

EXTRA_DIST += \
        $(srcdir)/include/utils/module.mk

