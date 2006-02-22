#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: Makefile.am 5019 2006-02-21 15:29:07Z dpage $
# Copyright (C) 2002 - 2006, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - src/main/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/main/events.cpp \
	$(srcdir)/main/dlgClasses.cpp

EXTRA_DIST += \
        $(srcdir)/main/module.mk \
	$(srcdir)/main/pgAdmin3.rc

