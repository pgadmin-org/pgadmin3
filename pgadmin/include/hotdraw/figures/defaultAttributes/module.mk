#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/hotdraw/figures/defaultAttributes Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/hotdraw/figures/defaultAttributes/hdFillAttribute.h \
	$(srcdir)/include/hotdraw/figures/defaultAttributes/hdFontAttribute.h \
	$(srcdir)/include/hotdraw/figures/defaultAttributes/hdFontColorAttribute.h \
	$(srcdir)/include/hotdraw/figures/defaultAttributes/hdLineAttribute.h

EXTRA_DIST += \
	$(srcdir)/include/hotdraw/figures/defaultAttributes/module.mk
