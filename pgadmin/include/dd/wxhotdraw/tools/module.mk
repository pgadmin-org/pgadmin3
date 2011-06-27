#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/dd/wxhotdraw/tools/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/dd/wxhotdraw/tools/wxhdAbstractTool.h \
	$(srcdir)/include/dd/wxhotdraw/tools/wxhdCanvasMenuTool.h \	
	$(srcdir)/include/dd/wxhotdraw/tools/wxhdCompositeFigureTool.h \
	$(srcdir)/include/dd/wxhotdraw/tools/wxhdConnectionCreationTool.h \
	$(srcdir)/include/dd/wxhotdraw/tools/wxhdCreationTool.h \
	$(srcdir)/include/dd/wxhotdraw/tools/wxhdDragCreationTool.h \
	$(srcdir)/include/dd/wxhotdraw/tools/wxhdDragTrackerTool.h \
	$(srcdir)/include/dd/wxhotdraw/tools/wxhdFigureTool.h \
	$(srcdir)/include/dd/wxhotdraw/tools/wxhdHandleTrackerTool.h \
	$(srcdir)/include/dd/wxhotdraw/tools/wxhdITool.h \
	$(srcdir)/include/dd/wxhotdraw/tools/wxhdMenuTool.h \
	$(srcdir)/include/dd/wxhotdraw/tools/wxhdPolyLineFigureTool.h \
	$(srcdir)/include/dd/wxhotdraw/tools/wxhdSelectAreaTool.h \
	$(srcdir)/include/dd/wxhotdraw/tools/wxhdSelectionTool.h \
	$(srcdir)/include/dd/wxhotdraw/tools/wxhdSimpleTextTool.h

EXTRA_DIST += \
	$(srcdir)/include/dd/wxhotdraw/tools/module.mk
