#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/wxhotdraw/utilities/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/dd/wxhotdraw/utilities/wxhdArrayCollection.cpp \
	$(srcdir)/dd/wxhotdraw/utilities/wxhdCollection.cpp \
	$(srcdir)/dd/wxhotdraw/utilities/wxhdGeometry.cpp \
	$(srcdir)/dd/wxhotdraw/utilities/wxhdKeyEvent.cpp \
	$(srcdir)/dd/wxhotdraw/utilities/wxhdMouseEvent.cpp \
	$(srcdir)/dd/wxhotdraw/utilities/wxhdMultiPosRect.cpp \
	$(srcdir)/dd/wxhotdraw/utilities/wxhdPoint.cpp \
	$(srcdir)/dd/wxhotdraw/utilities/wxhdRect.cpp \
	$(srcdir)/dd/wxhotdraw/utilities/wxhdRemoveDeleteDialog.cpp

EXTRA_DIST += \
	$(srcdir)/dd/wxhotdraw/utilities/module.mk
