#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/hotdraw/locator/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/hotdraw/locators/hdILocator.cpp \
	$(srcdir)/hotdraw/locators/hdPolyLineLocator.cpp

EXTRA_DIST += \
	$(srcdir)/hotdraw/locators/module.mk
