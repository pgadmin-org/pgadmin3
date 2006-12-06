#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2006, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - src/base/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/base/sysLogger.cpp \
	$(srcdir)/base/factory.cpp 

EXTRA_DIST += \
        $(srcdir)/base/module.mk
