#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2010, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/utils/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/utils/csvfiles.cpp \
	$(srcdir)/utils/factory.cpp \
	$(srcdir)/utils/favourites.cpp \
	$(srcdir)/utils/misc.cpp \
	$(srcdir)/utils/pgconfig.cpp \
	$(srcdir)/utils/registry.cpp \
	$(srcdir)/utils/sysLogger.cpp \
	$(srcdir)/utils/sysProcess.cpp \
	$(srcdir)/utils/sysSettings.cpp \
	$(srcdir)/utils/tabcomplete.c \
	$(srcdir)/utils/utffile.cpp \
	$(srcdir)/utils/macros.cpp

EXTRA_DIST += \
	$(srcdir)/utils/module.mk \
	$(srcdir)/utils/tab-complete.inc \
	$(srcdir)/utils/tabcomplete.pl
