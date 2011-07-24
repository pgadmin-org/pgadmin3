#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/dd/dditems/figures/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/dd/dditems/figures/ddColumnFigure.h \
	$(srcdir)/include/dd/dditems/figures/ddColumnKindIcon.h \
	$(srcdir)/include/dd/dditems/figures/ddColumnOptionIcon.h \
	$(srcdir)/include/dd/dditems/figures/ddRelationshipFigure.h \
	$(srcdir)/include/dd/dditems/figures/ddRelationshipTerminal.h \
	$(srcdir)/include/dd/dditems/figures/ddTableFigure.h \
	$(srcdir)/include/dd/dditems/figures/ddTextTableItemFigure.h

EXTRA_DIST += \
	$(srcdir)/include/dd/dditems/figures/module.mk

include $(srcdir)/include/dd/dditems/figures/xml/module.mk