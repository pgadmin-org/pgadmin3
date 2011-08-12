#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/hotdraw/handles/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/hotdraw/handles/hdButtonHandle.h \
	$(srcdir)/include/hotdraw/handles/hdChangeConnectionEndHandle.h \
	$(srcdir)/include/hotdraw/handles/hdChangeConnectionHandle.h \
	$(srcdir)/include/hotdraw/handles/hdChangeConnectionStartHandle.h \
	$(srcdir)/include/hotdraw/handles/hdIHandle.h \
	$(srcdir)/include/hotdraw/handles/hdLineConnectionHandle.h \
	$(srcdir)/include/hotdraw/handles/hdLocatorHandle.h \
	$(srcdir)/include/hotdraw/handles/hdPolyLineHandle.h

EXTRA_DIST += \
	$(srcdir)/include/hotdraw/handles/module.mk
