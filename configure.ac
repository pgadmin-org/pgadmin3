AC_PREREQ(2.57)
AC_REVISION($Revision$)
AC_COPYRIGHT(Copyright 2002, 2003, The pgAdmin3 Development Team)
AC_INIT(PgAdmin3, 0.1.1, pgadmin-support@postgresql.org, pgadmin3)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR([src/pgAdmin3.cpp])
AC_CONFIG_HEADER([config.h])
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

CHECK_WXWINDOWS
CHECK_LIBPQ

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
