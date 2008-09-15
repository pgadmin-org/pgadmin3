#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: pgadmin.sh,v 1.6 2008/07/27 16:25:05 pgunittest Exp $
# Copyright (C) 2002 - 2008, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - xtra/pgscript/file/test/ Makefile fragment
#
#######################################################################

EXTRA_DIST += \
	$(srcdir)/file/test/module.mk \
	$(srcdir)/file/test/execute.sh

include $(srcdir)/file/test/dictionary/module.mk
include $(srcdir)/file/test/source/module.mk
