#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/hotdraw/locator/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	include/hotdraw/locators/hdILocator.h \
	include/hotdraw/locators/hdPolyLineLocator.h

EXTRA_DIST += \
	include/hotdraw/locators/module.mk
