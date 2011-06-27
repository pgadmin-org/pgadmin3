#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/wxhotdraw/tools/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/dd/wxhotdraw/tools/wxhdAbstractTool.cpp \
	$(srcdir)/dd/wxhotdraw/tools/wxhdCanvasMenuTool.cpp \
	$(srcdir)/dd/wxhotdraw/tools/wxhdCompositeFigureTool.cpp \
	$(srcdir)/dd/wxhotdraw/tools/wxhdConnectionCreationTool.cpp \
	$(srcdir)/dd/wxhotdraw/tools/wxhdCreationTool.cpp \
	$(srcdir)/dd/wxhotdraw/tools/wxhdDragCreationTool.cpp \
	$(srcdir)/dd/wxhotdraw/tools/wxhdDragTrackerTool.cpp \
	$(srcdir)/dd/wxhotdraw/tools/wxhdFigureTool.cpp \
	$(srcdir)/dd/wxhotdraw/tools/wxhdHandleTrackerTool.cpp \
	$(srcdir)/dd/wxhotdraw/tools/wxhdITool.cpp \
	$(srcdir)/dd/wxhotdraw/tools/wxhdMenuTool.cpp \
	$(srcdir)/dd/wxhotdraw/tools/wxhdPolyLineFigureTool.cpp \
	$(srcdir)/dd/wxhotdraw/tools/wxhdSelectAreaTool.cpp \
	$(srcdir)/dd/wxhotdraw/tools/wxhdSelectionTool.cpp \
	$(srcdir)/dd/wxhotdraw/tools/wxhdSimpleTextTool.cpp

EXTRA_DIST += \
	$(srcdir)/dd/wxhotdraw/tools/module.mk
