#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/dditems/figures/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/dd/dditems/figures/ddColumnFigure.cpp \
	$(srcdir)/dd/dditems/figures/ddColumnKindIcon.cpp \
	$(srcdir)/dd/dditems/figures/ddColumnOptionIcon.cpp \
	$(srcdir)/dd/dditems/figures/ddRelationshipFigure.cpp \
	$(srcdir)/dd/dditems/figures/ddRelationshipItem.cpp \
	$(srcdir)/dd/dditems/figures/ddRelationshipTerminal.cpp \
	$(srcdir)/dd/dditems/figures/ddTableFigure.cpp \
	$(srcdir)/dd/dditems/figures/ddTextTableItemFigure.cpp

EXTRA_DIST += \
	$(srcdir)/dd/dditems/figures/module.mk

include $(srcdir)/dd/dditems/figures/xml/module.mk
