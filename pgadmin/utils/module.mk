#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/utils/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	utils/csvfiles.cpp \
	utils/factory.cpp \
	utils/favourites.cpp \
	utils/misc.cpp \
	utils/pgconfig.cpp \
	utils/registry.cpp \
	utils/sysLogger.cpp \
	utils/sysProcess.cpp \
	utils/sysSettings.cpp \
	utils/tabcomplete.c \
	utils/utffile.cpp \
	utils/macros.cpp

if BUILD_SSH_TUNNEL
pgadmin3_SOURCES += \
	utils/sshTunnel.cpp
endif

EXTRA_DIST += \
	utils/module.mk \
	utils/tab-complete.inc \
	utils/tabcomplete.pl
