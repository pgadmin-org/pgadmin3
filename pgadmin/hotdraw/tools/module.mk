#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/hotdraw/tools/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/hotdraw/tools/hdAbstractTool.cpp \
	$(srcdir)/hotdraw/tools/hdCanvasMenuTool.cpp \
	$(srcdir)/hotdraw/tools/hdCompositeFigureTool.cpp \
	$(srcdir)/hotdraw/tools/hdConnectionCreationTool.cpp \
	$(srcdir)/hotdraw/tools/hdCreationTool.cpp \
	$(srcdir)/hotdraw/tools/hdDragCreationTool.cpp \
	$(srcdir)/hotdraw/tools/hdDragTrackerTool.cpp \
	$(srcdir)/hotdraw/tools/hdFigureTool.cpp \
	$(srcdir)/hotdraw/tools/hdHandleTrackerTool.cpp \
	$(srcdir)/hotdraw/tools/hdITool.cpp \
	$(srcdir)/hotdraw/tools/hdMenuTool.cpp \
	$(srcdir)/hotdraw/tools/hdPolyLineFigureTool.cpp \
	$(srcdir)/hotdraw/tools/hdSelectAreaTool.cpp \
	$(srcdir)/hotdraw/tools/hdSelectionTool.cpp \
	$(srcdir)/hotdraw/tools/hdSimpleTextTool.cpp

EXTRA_DIST += \
	$(srcdir)/hotdraw/tools/module.mk
