#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/wxhotdraw/figures/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/dd/wxhotdraw/figures/wxhdAbstractFigure.cpp \
	$(srcdir)/dd/wxhotdraw/figures/wxhdAbstractMenuFigure.cpp \
	$(srcdir)/dd/wxhotdraw/figures/wxhdAttribute.cpp \
	$(srcdir)/dd/wxhotdraw/figures/wxhdAttributeFigure.cpp \
	$(srcdir)/dd/wxhotdraw/figures/wxhdBitmapFigure.cpp \
	$(srcdir)/dd/wxhotdraw/figures/wxhdCompositeFigure.cpp \
	$(srcdir)/dd/wxhotdraw/figures/wxhdIConnectionFigure.cpp \
	$(srcdir)/dd/wxhotdraw/figures/wxhdIFigure.cpp \
	$(srcdir)/dd/wxhotdraw/figures/wxhdLineConnection.cpp \
	$(srcdir)/dd/wxhotdraw/figures/wxhdLineTerminal.cpp \
	$(srcdir)/dd/wxhotdraw/figures/wxhdPolyLineFigure.cpp \
	$(srcdir)/dd/wxhotdraw/figures/wxhdRectangleFigure.cpp \
	$(srcdir)/dd/wxhotdraw/figures/wxhdSimpleTextFigure.cpp

EXTRA_DIST += \
	$(srcdir)/dd/wxhotdraw/figures/module.mk

include $(srcdir)/dd/wxhotdraw/figures/defaultAttributes/module.mk
include $(srcdir)/dd/wxhotdraw/figures/xml/module.mk
