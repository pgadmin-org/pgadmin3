#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/hotdraw/main/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	include/hotdraw/main/hdDrawing.h \
	include/hotdraw/main/hdDrawingEditor.h \
	include/hotdraw/main/hdDrawingView.h \
	include/hotdraw/main/hdObject.h

EXTRA_DIST += \
	include/hotdraw/main/module.mk
