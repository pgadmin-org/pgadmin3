#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/dd/wxhotdraw/figures/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/dd/wxhotdraw/figures/wxhdAbstractFigure.h \
	$(srcdir)/include/dd/wxhotdraw/figures/wxhdAbstractMenuFigure.h \
	$(srcdir)/include/dd/wxhotdraw/figures/wxhdAttribute.h \
	$(srcdir)/include/dd/wxhotdraw/figures/wxhdAttributeFigure.h \
	$(srcdir)/include/dd/wxhotdraw/figures/wxhdBitmapFigure.h \
	$(srcdir)/include/dd/wxhotdraw/figures/wxhdCompositeFigure.h \
	$(srcdir)/include/dd/wxhotdraw/figures/wxhdIConnectionFigure.h \
	$(srcdir)/include/dd/wxhotdraw/figures/wxhdIFigure.h \
	$(srcdir)/include/dd/wxhotdraw/figures/wxhdLineConnection.h \
	$(srcdir)/include/dd/wxhotdraw/figures/wxhdLineTerminal.h \
	$(srcdir)/include/dd/wxhotdraw/figures/wxhdPolyLineFigure.h \
	$(srcdir)/include/dd/wxhotdraw/figures/wxhdRectangleFigure.h \
	$(srcdir)/include/dd/wxhotdraw/figures/wxhdSimpleTextFigure.h

EXTRA_DIST += \
	$(srcdir)/include/dd/wxhotdraw/figures/module.mk

include $(srcdir)/include/dd/wxhotdraw/figures/defaultAttributes/module.mk
include $(srcdir)/include/dd/wxhotdraw/figures/xml/module.mk