#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2013, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/utild/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	include/utils/csvfiles.h \
	include/utils/factory.h \
	include/utils/favourites.h \
	include/utils/misc.h \
	include/utils/pgfeatures.h \
	include/utils/pgDefs.h \
	include/utils/pgconfig.h \
	include/utils/registry.h \
	include/utils/sysLogger.h \
	include/utils/sysProcess.h \
	include/utils/sysSettings.h \
	include/utils/utffile.h \
	include/utils/macros.h

EXTRA_DIST += \
        include/utils/module.mk

