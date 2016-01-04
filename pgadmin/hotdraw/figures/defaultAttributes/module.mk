#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/hotdraw/figures/defaultAttributes/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	hotdraw/figures/defaultAttributes/hdFillAttribute.cpp \
	hotdraw/figures/defaultAttributes/hdFontAttribute.cpp \
	hotdraw/figures/defaultAttributes/hdFontColorAttribute.cpp \
	hotdraw/figures/defaultAttributes/hdLineAttribute.cpp

EXTRA_DIST += \
	hotdraw/figures/defaultAttributes/module.mk
