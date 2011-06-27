#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/dditems/handles/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/dd/dditems/handles/ddAddColButtonHandle.cpp \
	$(srcdir)/dd/dditems/handles/ddAddFkButtonHandle.cpp \
	$(srcdir)/dd/dditems/handles/ddMinMaxTableButtonHandle.cpp \
	$(srcdir)/dd/dditems/handles/ddRemoveTableButtonHandle.cpp \
	$(srcdir)/dd/dditems/handles/ddScrollBarHandle.cpp \
	$(srcdir)/dd/dditems/handles/ddSouthTableSizeHandle.cpp

EXTRA_DIST += \
	$(srcdir)/dd/dditems/handles/module.mk
