#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/dd/wxhotdraw/handles/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/dd/wxhotdraw/handles/wxhdButtonHandle.h \
	$(srcdir)/include/dd/wxhotdraw/handles/wxhdChangeConnectionEndHandle.h \
	$(srcdir)/include/dd/wxhotdraw/handles/wxhdChangeConnectionHandle.h \
	$(srcdir)/include/dd/wxhotdraw/handles/wxhdChangeConnectionStartHandle.h \
	$(srcdir)/include/dd/wxhotdraw/handles/wxhdIHandle.h \
	$(srcdir)/include/dd/wxhotdraw/handles/wxhdLineConnectionHandle.h \
	$(srcdir)/include/dd/wxhotdraw/handles/wxhdLocatorHandle.h \
	$(srcdir)/include/dd/wxhotdraw/handles/wxhdPolyLineHandle.h

EXTRA_DIST += \
	$(srcdir)/include/dd/wxhotdraw/handles/module.mk
