#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/hotdraw/utilities/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/hotdraw/utilities/hdArrayCollection.h \
	$(srcdir)/include/hotdraw/utilities/hdCollection.h \
	$(srcdir)/include/hotdraw/utilities/hdCollectionBase.h \
	$(srcdir)/include/hotdraw/utilities/hdGeometry.h \
	$(srcdir)/include/hotdraw/utilities/hdKeyEvent.h \
	$(srcdir)/include/hotdraw/utilities/hdMultiPosRect.h \
	$(srcdir)/include/hotdraw/utilities/hdMouseEvent.h \
	$(srcdir)/include/hotdraw/utilities/hdPoint.h \
	$(srcdir)/include/hotdraw/utilities/hdRect.h \
	$(srcdir)/include/hotdraw/utilities/hdRemoveDeleteDialog.h

EXTRA_DIST += \
	$(srcdir)/include/hotdraw/utilities/module.mk
