##############################################################################
# pgAdmin III - PostgreSQL Tools
#
# Copyright (C) 2002 - 2009, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# Makefile - Makefile for *nix systems
##############################################################################

bin_PROGRAMS = pgScript

pgScript_SOURCES = \
	pgsMain.cpp

pgScript_LDADD += \
	$(builddir)/../lib/libpgs.a

if SUN_CXX
  __CXXFLAGS=""
else
  __CXXFLAGS=-Wall -Wno-non-virtual-dtor -fno-strict-aliasing
endif

AM_CXXFLAGS = $(__CXXFLAGS) -I$(top_srcdir)/pgadmin/include -DPGSCLI -DPGADMIN3_H
AM_CFLAGS = $(__CFLAGS) -I$(top_srcdir)/pgadmin/include -DPGSCLI -DPGADMIN3_H

# dist_man_MANS = pgScript.1
