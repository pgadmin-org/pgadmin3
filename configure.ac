AC_PREREQ(2.57)
AC_REVISION($Revision$)
AC_COPYRIGHT(Copyright 2002 - 2004 The pgAdmin Development Team)
AC_INIT(pgadmin3, 1.3.0, pgadmin-support@postgresql.org)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR([src/pgAdmin3.cpp])
AM_CONFIG_HEADER([config.h])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE
AC_PREFIX_DEFAULT([/usr/local/pgadmin3])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h netdb.h netinet/in.h stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_STRTOD
AC_CHECK_FUNCS([gethostbyname inet_ntoa memmove memset strchr])

# Custom checks
ENABLE_APPBUNDLE
AM_CONDITIONAL([APPBUNDLE], [test x$pg_appbundle = xyes])
CHECK_WX_CONFIG_BINARY
CHECK_PGSQL_INCLUDE
ENABLE_DEBUG
ENABLE_STATIC
CHECK_PGSQL
CHECK_WXWINDOWS

AC_CONFIG_FILES([Makefile
		 i18n/Makefile
	         src/Makefile
		 pkg/Makefile
		 pkg/debian/Makefile
		 pkg/fc1/Makefile
		 pkg/mac/Makefile
		 pkg/mandrake/Makefile
		 pkg/redhat/Makefile
		 pkg/slackware/Makefile
		 pkg/src/Makefile
		 pkg/win32/Makefile
		 xtra/Makefile
		 xtra/pgagent/Makefile])
AC_OUTPUT
