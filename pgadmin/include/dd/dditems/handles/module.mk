#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2013, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/dd/dditems/handles/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/dd/dditems/handles/ddAddColButtonHandle.h \
	$(srcdir)/include/dd/dditems/handles/ddAddFkButtonHandle.h \
	$(srcdir)/include/dd/dditems/handles/ddMinMaxTableButtonHandle.h \
	$(srcdir)/include/dd/dditems/handles/ddRemoveTableButtonHandle.h \
	$(srcdir)/include/dd/dditems/handles/ddScrollBarHandle.h \
	$(srcdir)/include/dd/dditems/handles/ddSouthTableSizeHandle.h

EXTRA_DIST += \
	$(srcdir)/include/dd/dditems/handles/module.mk
