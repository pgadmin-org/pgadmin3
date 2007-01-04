#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2007, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - src/utils/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/utils/factory.cpp \
	$(srcdir)/utils/favourites.cpp \
	$(srcdir)/utils/md5.cpp \
	$(srcdir)/utils/misc.cpp \
	$(srcdir)/utils/pgconfig.cpp \
	$(srcdir)/utils/sysLogger.cpp \
	$(srcdir)/utils/sysProcess.cpp \
	$(srcdir)/utils/sysSettings.cpp \
	$(srcdir)/utils/tabcomplete.c \
	$(srcdir)/utils/utffile.cpp

EXTRA_DIST += \
	$(srcdir)/utils/module.mk \
	$(srcdir)/utils/tab-complete.inc \
	$(srcdir)/utils/tabcomplete.pl
