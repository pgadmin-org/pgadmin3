#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/wxhotdraw/figures/defaultAttributes/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/dd/wxhotdraw/figures/defaultAttributes/wxhdFillAttribute.cpp \
	$(srcdir)/dd/wxhotdraw/figures/defaultAttributes/wxhdFontAttribute.cpp \
	$(srcdir)/dd/wxhotdraw/figures/defaultAttributes/wxhdFontColorAttribute.cpp \
	$(srcdir)/dd/wxhotdraw/figures/defaultAttributes/wxhdLineAttribute.cpp

EXTRA_DIST += \
	$(srcdir)/dd/wxhotdraw/figures/defaultAttributes/module.mk
