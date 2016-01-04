#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/dditems/figures/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	dd/dditems/figures/ddColumnFigure.cpp \
	dd/dditems/figures/ddColumnKindIcon.cpp \
	dd/dditems/figures/ddColumnOptionIcon.cpp \
	dd/dditems/figures/ddRelationshipFigure.cpp \
	dd/dditems/figures/ddRelationshipItem.cpp \
	dd/dditems/figures/ddRelationshipTerminal.cpp \
	dd/dditems/figures/ddTableFigure.cpp \
	dd/dditems/figures/ddTextTableItemFigure.cpp

EXTRA_DIST += \
	dd/dditems/figures/module.mk

include dd/dditems/figures/xml/module.mk
