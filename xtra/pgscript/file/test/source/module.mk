#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: pgadmin.sh,v 1.6 2008/07/27 16:25:05 pgunittest Exp $
# Copyright (C) 2002 - 2008, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - xtra/pgscript/file/test/source/ Makefile fragment
#
#######################################################################

EXTRA_DIST += \
	$(srcdir)/file/test/source/module.mk \
	$(srcdir)/file/test/source/calculations.sql.pgs \
	$(srcdir)/file/test/source/controls.sql.pgs \
	$(srcdir)/file/test/source/functions.sql.pgs \
	$(srcdir)/file/test/source/generators.sql.pgs \
	$(srcdir)/file/test/source/procedures.sql.pgs \
	$(srcdir)/file/test/source/randdata.sql.pgs \
	$(srcdir)/file/test/source/records.sql.pgs \
	$(srcdir)/file/test/source/reference.sql.pgs
