#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/hotdraw/locator/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	hotdraw/locators/hdILocator.cpp \
	hotdraw/locators/hdPolyLineLocator.cpp

EXTRA_DIST += \
	hotdraw/locators/module.mk
