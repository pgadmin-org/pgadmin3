#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/dd/wxhotdraw/locator/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/dd/wxhotdraw/locators/wxhdILocator.h \
	$(srcdir)/include/dd/wxhotdraw/locators/wxhdPolyLineLocator.h

EXTRA_DIST += \
	$(srcdir)/include/dd/wxhotdraw/locators/module.mk
