#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/hotdraw/connectors/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	include/hotdraw/connectors/hdChopBoxConnector.h \
	include/hotdraw/connectors/hdIConnector.h \
	include/hotdraw/connectors/hdLocatorConnector.h \
	include/hotdraw/connectors/hdStickyRectangleConnector.h

EXTRA_DIST += \
	include/hotdraw/connectors/module.mk
