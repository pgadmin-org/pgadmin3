#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/dd/wxhotdraw/connectors/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/dd/wxhotdraw/connectors/wxhdChopBoxConnector.h \
	$(srcdir)/include/dd/wxhotdraw/connectors/wxhdIConnector.h \
	$(srcdir)/include/dd/wxhotdraw/connectors/wxhdLocatorConnector.h \
	$(srcdir)/include/dd/wxhotdraw/connectors/wxhdStickyRectangleConnector.h

EXTRA_DIST += \
	$(srcdir)/include/dd/wxhotdraw/connectors/module.mk
