#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/dditems/tools/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	dd/dditems/tools/ddColumnFigureTool.cpp \
	dd/dditems/tools/ddColumnTextTool.cpp

EXTRA_DIST += \
	dd/dditems/tools/module.mk
