#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/hotdraw/main/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/hotdraw/main/hdDrawing.cpp \
	$(srcdir)/hotdraw/main/hdDrawingEditor.cpp \
	$(srcdir)/hotdraw/main/hdDrawingView.cpp

EXTRA_DIST += \
	$(srcdir)/hotdraw/main/module.mk
