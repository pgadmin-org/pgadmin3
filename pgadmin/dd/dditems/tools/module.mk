#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/dditems/tools/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/dd/dditems/tools/ddColumnFigureTool.cpp \
	$(srcdir)/dd/dditems/tools/ddColumnTextTool.cpp

EXTRA_DIST += \
	$(srcdir)/dd/dditems/tools/module.mk
