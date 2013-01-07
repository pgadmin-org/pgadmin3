#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2013, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/dd/dditems/tools/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/dd/dditems/tools/ddColumnFigureTool.h \
	$(srcdir)/include/dd/dditems/tools/ddColumnTextTool.h

EXTRA_DIST += \
	$(srcdir)/include/dd/dditems/tools/module.mk
