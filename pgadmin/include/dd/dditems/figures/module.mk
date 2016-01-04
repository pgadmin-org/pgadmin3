#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/dd/dditems/figures/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	include/dd/dditems/figures/ddColumnFigure.h \
	include/dd/dditems/figures/ddColumnKindIcon.h \
	include/dd/dditems/figures/ddColumnOptionIcon.h \
	include/dd/dditems/figures/ddRelationshipFigure.h \
	include/dd/dditems/figures/ddRelationshipItem.h \
	include/dd/dditems/figures/ddRelationshipTerminal.h \
	include/dd/dditems/figures/ddTableFigure.h \
	include/dd/dditems/figures/ddTextTableItemFigure.h

EXTRA_DIST += \
	include/dd/dditems/figures/module.mk

include include/dd/dditems/figures/xml/module.mk
