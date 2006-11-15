MODULE_big = admin
PG_CPPFLAGS = -I$(libpq_srcdir)
DATA_built = admin.sql
DOCS = README.admin
OBJS = genfile.o misc.o

ifdef USE_PGXS
PGXS := $(shell pg_config --pgxs)
include $(PGXS)
else
subdir = contrib/admin81
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
