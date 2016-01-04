##############################################################################
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# Makefile - Makefile for *nix systems
##############################################################################

DIST_SUBDIRS = png2c wx-build
SUBDIRS = wx-build

clean:
	for dir in $(DIST_SUBDIRS); do \
		$(MAKE) -C $$dir $(AM_MAKEFLAGS) clean || exit 1; \
	done;
