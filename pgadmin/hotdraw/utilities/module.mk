#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/hotdraw/utilities/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/hotdraw/utilities/hdArrayCollection.cpp \
	$(srcdir)/hotdraw/utilities/hdCollection.cpp \
	$(srcdir)/hotdraw/utilities/hdGeometry.cpp \
	$(srcdir)/hotdraw/utilities/hdKeyEvent.cpp \
	$(srcdir)/hotdraw/utilities/hdMouseEvent.cpp \
	$(srcdir)/hotdraw/utilities/hdMultiPosRect.cpp \
	$(srcdir)/hotdraw/utilities/hdPoint.cpp \
	$(srcdir)/hotdraw/utilities/hdRect.cpp \
	$(srcdir)/hotdraw/utilities/hdRemoveDeleteDialog.cpp

EXTRA_DIST += \
	$(srcdir)/hotdraw/utilities/module.mk
