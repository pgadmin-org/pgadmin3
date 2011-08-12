#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/hotdraw/tools/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/hotdraw/tools/hdAbstractTool.h \
	$(srcdir)/include/hotdraw/tools/hdCanvasMenuTool.h \
	$(srcdir)/include/hotdraw/tools/hdCompositeFigureTool.h \
	$(srcdir)/include/hotdraw/tools/hdConnectionCreationTool.h \
	$(srcdir)/include/hotdraw/tools/hdCreationTool.h \
	$(srcdir)/include/hotdraw/tools/hdDragCreationTool.h \
	$(srcdir)/include/hotdraw/tools/hdDragTrackerTool.h \
	$(srcdir)/include/hotdraw/tools/hdFigureTool.h \
	$(srcdir)/include/hotdraw/tools/hdHandleTrackerTool.h \
	$(srcdir)/include/hotdraw/tools/hdITool.h \
	$(srcdir)/include/hotdraw/tools/hdMenuTool.h \
	$(srcdir)/include/hotdraw/tools/hdPolyLineFigureTool.h \
	$(srcdir)/include/hotdraw/tools/hdSelectAreaTool.h \
	$(srcdir)/include/hotdraw/tools/hdSelectionTool.h \
	$(srcdir)/include/hotdraw/tools/hdSimpleTextTool.h

EXTRA_DIST += \
	$(srcdir)/include/hotdraw/tools/module.mk
