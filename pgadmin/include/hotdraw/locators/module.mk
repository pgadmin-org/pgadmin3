#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/hotdraw/locator/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/hotdraw/locators/hdILocator.h \
	$(srcdir)/include/hotdraw/locators/hdPolyLineLocator.h

EXTRA_DIST += \
	$(srcdir)/include/hotdraw/locators/module.mk
