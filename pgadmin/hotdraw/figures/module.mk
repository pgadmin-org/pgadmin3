#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/hotdraw/figures/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	hotdraw/figures/hdAbstractFigure.cpp \
	hotdraw/figures/hdAbstractMenuFigure.cpp \
	hotdraw/figures/hdAttribute.cpp \
	hotdraw/figures/hdAttributeFigure.cpp \
	hotdraw/figures/hdBitmapFigure.cpp \
	hotdraw/figures/hdCompositeFigure.cpp \
	hotdraw/figures/hdIConnectionFigure.cpp \
	hotdraw/figures/hdIFigure.cpp \
	hotdraw/figures/hdLineConnection.cpp \
	hotdraw/figures/hdLineTerminal.cpp \
	hotdraw/figures/hdPolyLineFigure.cpp \
	hotdraw/figures/hdRectangleFigure.cpp \
	hotdraw/figures/hdSimpleTextFigure.cpp

EXTRA_DIST += \
	hotdraw/figures/module.mk

include hotdraw/figures/defaultAttributes/module.mk
include hotdraw/figures/xml/module.mk
