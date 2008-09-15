#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: pgadmin.sh,v 1.6 2008/07/27 16:25:05 pgunittest Exp $
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
