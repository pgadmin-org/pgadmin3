#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/hotdraw/figures/defaultAttributes Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	include/hotdraw/figures/defaultAttributes/hdFillAttribute.h \
	include/hotdraw/figures/defaultAttributes/hdFontAttribute.h \
	include/hotdraw/figures/defaultAttributes/hdFontColorAttribute.h \
	include/hotdraw/figures/defaultAttributes/hdLineAttribute.h

EXTRA_DIST += \
	include/hotdraw/figures/defaultAttributes/module.mk
