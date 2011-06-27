#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/dd/wxhotdraw/main/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/dd/wxhotdraw/main/wxhdDrawing.h \
	$(srcdir)/include/dd/wxhotdraw/main/wxhdDrawingEditor.h \
	$(srcdir)/include/dd/wxhotdraw/main/wxhdDrawingView.h \
	$(srcdir)/include/dd/wxhotdraw/main/wxhdObject.h

EXTRA_DIST += \
	$(srcdir)/include/dd/wxhotdraw/main/module.mk
