MODULE_big = admin81
PG_CPPFLAGS = -I$(libpq_srcdir)
DATA_built = admin81.sql
DOCS = README.admin81
OBJS = admin81.o

ifdef USE_PGXS
PGXS := $(shell pg_config --pgxs)
include $(PGXS)
else
subdir = contrib/admin81
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
