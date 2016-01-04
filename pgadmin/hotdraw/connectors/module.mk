#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/hotdraw/connectors/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	hotdraw/connectors/hdChopBoxConnector.cpp \
	hotdraw/connectors/hdIConnector.cpp \
	hotdraw/connectors/hdLocatorConnector.cpp \
	hotdraw/connectors/hdStickyRectangleConnector.cpp

EXTRA_DIST += \
	hotdraw/connectors/module.mk
