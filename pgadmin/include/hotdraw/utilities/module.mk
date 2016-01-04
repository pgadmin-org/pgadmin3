#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/hotdraw/utilities/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	include/hotdraw/utilities/hdArrayCollection.h \
	include/hotdraw/utilities/hdCollection.h \
	include/hotdraw/utilities/hdCollectionBase.h \
	include/hotdraw/utilities/hdGeometry.h \
	include/hotdraw/utilities/hdKeyEvent.h \
	include/hotdraw/utilities/hdMultiPosRect.h \
	include/hotdraw/utilities/hdMouseEvent.h \
	include/hotdraw/utilities/hdPoint.h \
	include/hotdraw/utilities/hdRect.h \
	include/hotdraw/utilities/hdRemoveDeleteDialog.h

EXTRA_DIST += \
	include/hotdraw/utilities/module.mk
