#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/wxhotdraw/main/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/dd/wxhotdraw/main/wxhdDrawing.cpp \
	$(srcdir)/dd/wxhotdraw/main/wxhdDrawingEditor.cpp \
	$(srcdir)/dd/wxhotdraw/main/wxhdDrawingView.cpp

EXTRA_DIST += \
	$(srcdir)/dd/wxhotdraw/main/module.mk
