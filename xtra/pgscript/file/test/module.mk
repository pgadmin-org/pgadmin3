#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2009, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - xtra/pgscript/file/test/ Makefile fragment
#
#######################################################################

EXTRA_DIST += \
	$(srcdir)/file/test/module.mk \
	$(srcdir)/file/test/execute.sh

include $(srcdir)/file/test/dictionary/module.mk
include $(srcdir)/file/test/source/module.mk
