#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2012, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/hotdraw/figures/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/hotdraw/figures/hdAbstractFigure.h \
	$(srcdir)/include/hotdraw/figures/hdAbstractMenuFigure.h \
	$(srcdir)/include/hotdraw/figures/hdAttribute.h \
	$(srcdir)/include/hotdraw/figures/hdAttributeFigure.h \
	$(srcdir)/include/hotdraw/figures/hdBitmapFigure.h \
	$(srcdir)/include/hotdraw/figures/hdCompositeFigure.h \
	$(srcdir)/include/hotdraw/figures/hdIConnectionFigure.h \
	$(srcdir)/include/hotdraw/figures/hdIFigure.h \
	$(srcdir)/include/hotdraw/figures/hdLineConnection.h \
	$(srcdir)/include/hotdraw/figures/hdLineTerminal.h \
	$(srcdir)/include/hotdraw/figures/hdPolyLineFigure.h \
	$(srcdir)/include/hotdraw/figures/hdRectangleFigure.h \
	$(srcdir)/include/hotdraw/figures/hdSimpleTextFigure.h

EXTRA_DIST += \
	$(srcdir)/include/hotdraw/figures/module.mk

include $(srcdir)/include/hotdraw/figures/defaultAttributes/module.mk
include $(srcdir)/include/hotdraw/figures/xml/module.mk
