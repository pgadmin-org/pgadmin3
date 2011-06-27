#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/wxhotdraw/handles/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/dd/wxhotdraw/handles/wxhdButtonHandle.cpp \
	$(srcdir)/dd/wxhotdraw/handles/wxhdChangeConnectionEndHandle.cpp \
	$(srcdir)/dd/wxhotdraw/handles/wxhdChangeConnectionHandle.cpp \
	$(srcdir)/dd/wxhotdraw/handles/wxhdChangeConnectionStartHandle.cpp \
	$(srcdir)/dd/wxhotdraw/handles/wxhdIHandle.cpp \
	$(srcdir)/dd/wxhotdraw/handles/wxhdLineConnectionHandle.cpp \
	$(srcdir)/dd/wxhotdraw/handles/wxhdLocatorHandle.cpp \
	$(srcdir)/dd/wxhotdraw/handles/wxhdPolyLineHandle.cpp

EXTRA_DIST += \
	$(srcdir)/dd/wxhotdraw/handles/module.mk
