#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/hotdraw/tools/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	hotdraw/tools/hdAbstractTool.cpp \
	hotdraw/tools/hdCanvasMenuTool.cpp \
	hotdraw/tools/hdCompositeFigureTool.cpp \
	hotdraw/tools/hdConnectionCreationTool.cpp \
	hotdraw/tools/hdCreationTool.cpp \
	hotdraw/tools/hdDragCreationTool.cpp \
	hotdraw/tools/hdDragTrackerTool.cpp \
	hotdraw/tools/hdFigureTool.cpp \
	hotdraw/tools/hdHandleTrackerTool.cpp \
	hotdraw/tools/hdITool.cpp \
	hotdraw/tools/hdMenuTool.cpp \
	hotdraw/tools/hdPolyLineFigureTool.cpp \
	hotdraw/tools/hdSelectAreaTool.cpp \
	hotdraw/tools/hdSelectionTool.cpp \
	hotdraw/tools/hdSimpleTextTool.cpp

EXTRA_DIST += \
	hotdraw/tools/module.mk
