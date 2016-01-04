#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/hotdraw/figures/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	include/hotdraw/figures/hdAbstractFigure.h \
	include/hotdraw/figures/hdAbstractMenuFigure.h \
	include/hotdraw/figures/hdAttribute.h \
	include/hotdraw/figures/hdAttributeFigure.h \
	include/hotdraw/figures/hdBitmapFigure.h \
	include/hotdraw/figures/hdCompositeFigure.h \
	include/hotdraw/figures/hdIConnectionFigure.h \
	include/hotdraw/figures/hdIFigure.h \
	include/hotdraw/figures/hdLineConnection.h \
	include/hotdraw/figures/hdLineTerminal.h \
	include/hotdraw/figures/hdPolyLineFigure.h \
	include/hotdraw/figures/hdRectangleFigure.h \
	include/hotdraw/figures/hdSimpleTextFigure.h

EXTRA_DIST += \
	include/hotdraw/figures/module.mk

include include/hotdraw/figures/defaultAttributes/module.mk
include include/hotdraw/figures/xml/module.mk
