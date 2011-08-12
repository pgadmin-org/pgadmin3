#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/hotdraw/connectors/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/hotdraw/connectors/hdChopBoxConnector.cpp \
	$(srcdir)/hotdraw/connectors/hdIConnector.cpp \
	$(srcdir)/hotdraw/connectors/hdLocatorConnector.cpp \
	$(srcdir)/hotdraw/connectors/hdStickyRectangleConnector.cpp

EXTRA_DIST += \
	$(srcdir)/hotdraw/connectors/module.mk
