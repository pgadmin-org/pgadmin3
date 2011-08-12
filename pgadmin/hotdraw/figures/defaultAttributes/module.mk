#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/hotdraw/figures/defaultAttributes/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/hotdraw/figures/defaultAttributes/hdFillAttribute.cpp \
	$(srcdir)/hotdraw/figures/defaultAttributes/hdFontAttribute.cpp \
	$(srcdir)/hotdraw/figures/defaultAttributes/hdFontColorAttribute.cpp \
	$(srcdir)/hotdraw/figures/defaultAttributes/hdLineAttribute.cpp

EXTRA_DIST += \
	$(srcdir)/hotdraw/figures/defaultAttributes/module.mk
