#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/hotdraw/main/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/hotdraw/main/hdDrawing.h \
	$(srcdir)/include/hotdraw/main/hdDrawingEditor.h \
	$(srcdir)/include/hotdraw/main/hdDrawingView.h \
	$(srcdir)/include/hotdraw/main/hdObject.h

EXTRA_DIST += \
	$(srcdir)/include/hotdraw/main/module.mk
