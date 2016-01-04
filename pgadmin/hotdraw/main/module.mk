#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/hotdraw/main/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	hotdraw/main/hdDrawing.cpp \
	hotdraw/main/hdDrawingEditor.cpp \
	hotdraw/main/hdDrawingView.cpp

EXTRA_DIST += \
	hotdraw/main/module.mk
