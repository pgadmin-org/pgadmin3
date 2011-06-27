#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/wxhotdraw/connectors/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/dd/wxhotdraw/connectors/wxhdChopBoxConnector.cpp \
	$(srcdir)/dd/wxhotdraw/connectors/wxhdIConnector.cpp \
	$(srcdir)/dd/wxhotdraw/connectors/wxhdLocatorConnector.cpp \
	$(srcdir)/dd/wxhotdraw/connectors/wxhdStickyRectangleConnector.cpp

EXTRA_DIST += \
	$(srcdir)/dd/wxhotdraw/connectors/module.mk
