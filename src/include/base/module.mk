#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2006, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - src/include/base/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/base/appbase.h \
	$(srcdir)/include/base/base.h \
	$(srcdir)/include/base/factory.h \
	$(srcdir)/include/base/pgConnBase.h \
	$(srcdir)/include/base/pgDefs.h \
	$(srcdir)/include/base/pgSetBase.h \
	$(srcdir)/include/base/sysLogger.h

EXTRA_DIST += \
        $(srcdir)/include/base/module.mk

