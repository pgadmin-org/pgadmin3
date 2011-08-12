#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/hotdraw/connectors/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/hotdraw/connectors/hdChopBoxConnector.h \
	$(srcdir)/include/hotdraw/connectors/hdIConnector.h \
	$(srcdir)/include/hotdraw/connectors/hdLocatorConnector.h \
	$(srcdir)/include/hotdraw/connectors/hdStickyRectangleConnector.h

EXTRA_DIST += \
	$(srcdir)/include/hotdraw/connectors/module.mk
