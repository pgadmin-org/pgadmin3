#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/wxhotdraw/locator/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/dd/wxhotdraw/locators/wxhdILocator.cpp \
	$(srcdir)/dd/wxhotdraw/locators/wxhdPolyLineLocator.cpp

EXTRA_DIST += \
	$(srcdir)/dd/wxhotdraw/locators/module.mk
