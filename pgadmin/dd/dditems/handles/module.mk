#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/dditems/handles/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	dd/dditems/handles/ddAddColButtonHandle.cpp \
	dd/dditems/handles/ddAddFkButtonHandle.cpp \
	dd/dditems/handles/ddMinMaxTableButtonHandle.cpp \
	dd/dditems/handles/ddRemoveTableButtonHandle.cpp \
	dd/dditems/handles/ddScrollBarHandle.cpp \
	dd/dditems/handles/ddSouthTableSizeHandle.cpp

EXTRA_DIST += \
	dd/dditems/handles/module.mk
