##############################################################################
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2006, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# Makefile - Makefile for *nix systems
##############################################################################

if !APPBUNDLE

SUBDIRS = pgagent

endif

# Note: We cannot add a Makefile to the admin directory as it already
#       contains one to be used as part of the PostreSQL contrib build
#	system. Instead, we'll add the files in that directory here.

EXTRA_DIST = \
	$(srcdir)/admin/Makefile \
	$(srcdir)/admin/README.admin \
	$(srcdir)/admin/admin.sql.in \
	$(srcdir)/admin/genfile.c \
	$(srcdir)/admin/misc.c \
	$(srcdir)/admin81/Makefile \
	$(srcdir)/admin81/README.admin81 \
	$(srcdir)/admin81/admin81.sql.in \
	$(srcdir)/admin81/admin81.c
