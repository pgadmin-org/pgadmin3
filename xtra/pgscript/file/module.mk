#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2008, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - xtra/pgscript/file/ Makefile fragment
#
#######################################################################

EXTRA_DIST += \
	$(srcdir)/file/module.mk

include $(srcdir)/file/input/module.mk
include $(srcdir)/file/test/module.mk
