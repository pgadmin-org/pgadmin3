#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: Makefile.am 5019 2006-02-21 15:29:07Z dpage $
# Copyright (C) 2002 - 2006, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - src/utils/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/utils/favourites.cpp \
	$(srcdir)/utils/md5.cpp \
	$(srcdir)/utils/misc.cpp \
	$(srcdir)/utils/pgconfig.cpp \
	$(srcdir)/utils/sysProcess.cpp \
	$(srcdir)/utils/sysSettings.cpp \
	$(srcdir)/utils/tabcomplete.c \
	$(srcdir)/utils/update.cpp \
	$(srcdir)/utils/utffile.cpp

EXTRA_DIST += \
	$(srcdir)/utils/module.mk \
	$(srcdir)/utils/precomp.cpp \
	$(srcdir)/utils/tab-complete.inc \
	$(srcdir)/utils/tabcomplete.pl
