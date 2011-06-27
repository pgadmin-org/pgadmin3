#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/dd/wxhotdraw/figures/defaultAttributes Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/dd/wxhotdraw/figures/defaultAttributes/wxhdFillAttribute.h \
	$(srcdir)/include/dd/wxhotdraw/figures/defaultAttributes/wxhdFontAttribute.h \
	$(srcdir)/include/dd/wxhotdraw/figures/defaultAttributes/wxhdFontColorAttribute.h \
	$(srcdir)/include/dd/wxhotdraw/figures/defaultAttributes/wxhdLineAttribute.h

EXTRA_DIST += \
	$(srcdir)/include/dd/wxhotdraw/figures/defaultAttributes/module.mk
