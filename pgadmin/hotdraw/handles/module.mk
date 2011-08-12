#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/hotdraw/handles/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/hotdraw/handles/hdButtonHandle.cpp \
	$(srcdir)/hotdraw/handles/hdChangeConnectionEndHandle.cpp \
	$(srcdir)/hotdraw/handles/hdChangeConnectionHandle.cpp \
	$(srcdir)/hotdraw/handles/hdChangeConnectionStartHandle.cpp \
	$(srcdir)/hotdraw/handles/hdIHandle.cpp \
	$(srcdir)/hotdraw/handles/hdLineConnectionHandle.cpp \
	$(srcdir)/hotdraw/handles/hdLocatorHandle.cpp \
	$(srcdir)/hotdraw/handles/hdPolyLineHandle.cpp

EXTRA_DIST += \
	$(srcdir)/hotdraw/handles/module.mk
