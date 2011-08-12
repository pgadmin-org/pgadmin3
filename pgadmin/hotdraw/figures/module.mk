#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/hotdraw/figures/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/hotdraw/figures/hdAbstractFigure.cpp \
	$(srcdir)/hotdraw/figures/hdAbstractMenuFigure.cpp \
	$(srcdir)/hotdraw/figures/hdAttribute.cpp \
	$(srcdir)/hotdraw/figures/hdAttributeFigure.cpp \
	$(srcdir)/hotdraw/figures/hdBitmapFigure.cpp \
	$(srcdir)/hotdraw/figures/hdCompositeFigure.cpp \
	$(srcdir)/hotdraw/figures/hdIConnectionFigure.cpp \
	$(srcdir)/hotdraw/figures/hdIFigure.cpp \
	$(srcdir)/hotdraw/figures/hdLineConnection.cpp \
	$(srcdir)/hotdraw/figures/hdLineTerminal.cpp \
	$(srcdir)/hotdraw/figures/hdPolyLineFigure.cpp \
	$(srcdir)/hotdraw/figures/hdRectangleFigure.cpp \
	$(srcdir)/hotdraw/figures/hdSimpleTextFigure.cpp

EXTRA_DIST += \
	$(srcdir)/hotdraw/figures/module.mk

include $(srcdir)/hotdraw/figures/defaultAttributes/module.mk
include $(srcdir)/hotdraw/figures/xml/module.mk
