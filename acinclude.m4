#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2005, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# acinclude.m4 - Build system scripts.
#
#######################################################################

#############################
# Override wxWidgets version #
#############################
AC_DEFUN([SET_WX_VERSION],
[
	AC_ARG_WITH(wx-version, [  --with-wx-version=<version number>  the wxWidgets version in major.minor format (default: 2.6)],
	[
		if test "$withval" == yes; then
			AC_MSG_ERROR([you must specify a version number when using --with-wx-version=<version number>])
		else
			if test -z "$withval"; then
				AC_MSG_ERROR([you must specify a version number when using --with-wx-version=<version number>])
			else
				WX_VERSION="$withval"
			fi
		fi
	],
	[
		WX_VERSION="2.6"
	])
])

####################
# Locate wxWidgets #
####################
AC_DEFUN([LOCATE_WXWIDGETS],
[
	AC_ARG_WITH(wx, [  --with-wx=DIR       root directory for wxWidgets installation],
	[
		if test "$withval" != no
		then
			WX_HOME="$withval"
                        if test ! -f "${WX_HOME}/bin/wx-config"
                        then
                                AC_MSG_ERROR([Could not find your wxWidgets installation in ${WX_HOME}])
                        fi

		fi
		WX_CONFIG=${WX_HOME}/bin/wx-config
	], 
	[
		WX_HOME=/usr/local/wx2
		if test ! -f "${WX_HOME}/bin/wx-config"
		then
			WX_HOME=/usr/local
			if test ! -f "${WX_HOME}/bin/wx-config"
			then
				WX_HOME=/usr
				if test ! -f "${WX_HOME}/bin/wx-config"
				then
					AC_MSG_ERROR([Could not find your wxWidgets installation. You might need to use the --with-wx=DIR configure option])
				fi
			fi
		fi
		WX_CONFIG=${WX_HOME}/bin/wx-config
	])
])

#####################
# Locate PostgreSQL #
#####################
AC_DEFUN([LOCATE_POSTGRESQL],
[
	AC_ARG_WITH(pgsql, [  --with-pgsql=DIR    root directory for PostgreSQL installation],
	[
		if test "$withval" != no
		then
			PG_HOME="$withval"
                        if test ! -f "${PG_HOME}/bin/pg_config"
                        then
                                AC_MSG_ERROR([Could not find your PostgreSQL installation in ${PG_HOME}])
                        fi

		fi
		PG_CONFIG=${PG_HOME}/bin/pg_config
	],
	[
		PG_HOME=/usr/local/pgsql
		if test ! -f "${PG_HOME}/bin/pg_config"
		then
			PG_HOME=/usr/local
			if test ! -f "${PG_HOME}/bin/pg_config"
			then
				PG_HOME=/usr
				if test ! -f "${PG_HOME}/bin/pg_config"
				then
					AC_MSG_ERROR([Could not find your PostgreSQL installation. You might need to use the --with-pgsql=DIR configure option])
				fi
			fi
		fi
		PG_CONFIG=${PG_HOME}/bin/pg_config
	])
])

###########################
# Debug build of pgAdmin3 #
###########################
AC_DEFUN([ENABLE_DEBUG],
[
	AC_ARG_ENABLE(debug, [  --enable-debug       build a debug version of pgAdmin3],
	[
		if test "$enableval" = yes
		then
			BUILD_DEBUG=yes
		else
			BUILD_DEBUG=no
		fi
	],
	[
		BUILD_DEBUG=no
	])
])
AC_SUBST(BUILD_DEBUG)

############################
# Static build of pgAdmin3 #
############################
AC_DEFUN([ENABLE_STATIC],
[
	AC_ARG_ENABLE(static, [  --enable-static      build a statically linked version of pgAdmin3],
	[
		if test "$enableval" = yes
		then
			BUILD_STATIC=yes
			WX_STATIC="--static=yes"
		else
			BUILD_STATIC=no
			WX_STATIC="--static=no"
		fi
	],
	[
		BUILD_STATIC=no
		WX_STATIC="--static=no"
	])
])

############################
# Build an pgAdmin III.app  #
############################
AC_DEFUN([ENABLE_APPBUNDLE],
[
	AC_ARG_ENABLE(appbundle, [  --enable-appbundle   Build a Mac OS X appbundle],
	[
		if test "$enableval" = yes
		then
			BUILD_APPBUNDLE=yes
			prefix=$(pwd)/tmp
			bundledir="$(pwd)/pgAdmin3.app"
			bindir="$bundledir/Contents/MacOS"
			datadir="$bundledir/Contents/SharedSupport"
			AC_SUBST(bundledir)
		else
			BUILD_APPBUNDLE=no
		fi
	],
	[
		BUILD_APPBUNDLE=no
	])
])

#########################################
# Check for libpq libraries and headers #
#########################################
AC_DEFUN([SETUP_POSTGRESQL],
[
	if test -n "${PG_HOME}"
	then
		PGSQL_OLD_LDFLAGS="$LDFLAGS"
		PGSQL_OLD_CPPFLAGS="$CPPFLAGS"

		# Solaris needs -lssl for this test
		case "${host}" in
			*solaris*)
				LDFLAGS="$LDFLAGS -L${PG_HOME}/lib -lssl"
				;;
			*)
				LDFLAGS="$LDFLAGS -L${PG_HOME}/lib"
				;;
		esac

		AC_LANG_SAVE
		AC_LANG_C
		AC_CHECK_LIB(pq, PQexec, [PG_LIBPQ=yes], [PG_LIBPQ=no])

		if test "$build_cpu-$build_vendor" = "powerpc-apple"
		then
			echo -n "Checking if libpq links against libssl: "
			if test "$(otool -L ${PG_HOME}/lib/libpq.?.dylib | grep -c libssl)" -gt 0
			then
				PG_SSL="yes"
			else
				PG_SSL="no"
			fi
			echo $PG_SSL
		else
			AC_CHECK_LIB(pq, SSL_connect, [PG_SSL=yes], [PG_SSL=no])
		fi
		AC_LANG_RESTORE

		PG_INCLUDE=`${PG_CONFIG} --includedir` 
		CPPFLAGS="$CPPFLAGS -I${PG_INCLUDE}"
	
		PG_LIB=`${PG_CONFIG} --libdir`

		PG_VERSION=`${PG_CONFIG} --version`
		
		if test "$BUILD_STATIC" = "yes"
		then
			if test "$build_cpu-$build_vendor" = "powerpc-apple"
			then
				CRYPT_LIB=""
			else
				CRYPT_LIB="-lcrypt"
			fi

			if test "$PG_SSL" = "yes"
			then
				LIBS="${PG_LIB}/libpq.a $CRYPT_LIB $LIBS -lssl -lcrypto"
			else
				LIBS="${PG_LIB}/libpq.a $CRYPT_LIB $LIBS -lcrypto"
			fi
		else
			if test "$PG_SSL" = "yes"
			then
				LIBS="$LIBS -L${PG_LIB} -lssl -lcrypto -lpq"
			else
				LIBS="$LIBS -L${PG_LIB} -lcrypto -lpq"
			fi
		fi

		AC_LANG_SAVE
		AC_LANG_C
		AC_CHECK_HEADER(libpq-fe.h, [PG_LIBPQFE=yes], [PG_LIBPQFE=no])
		AC_LANG_RESTORE

		if test "$PG_LIBPQ" = "yes" -a "$PG_LIBPQ" = "yes"
		then
			AC_MSG_CHECKING(PostgreSQL in ${PG_HOME})
			AC_MSG_RESULT(ok)
		else
			AC_MSG_CHECKING(PostgreSQL in ${PG_HOME})
			AC_MSG_RESULT(failed)
			LDFLAGS="$PGSQL_OLD_LDFLAGS"
			CPPFLAGS="$PGSQL_OLD_CPPFLAGS"
			AC_MSG_ERROR([you must specify a valid PostgreSQL installation with --with-pgsql=DIR])
		fi

		if test "$PG_SSL" = "yes"
		then
			CPPFLAGS="$CPPFLAGS -DSSL"
		fi
	fi
])
AC_SUBST(PG_CONFIG)

################################################
# Check for wxWidgets libraries and headers    #
################################################
AC_DEFUN([SETUP_WXWIDGETS],
[
	if test -n "${WX_HOME}"
	then
		LDFLAGS="$LDFLAGS -L${WX_HOME}/lib"
		WX_OLD_LDFLAGS="$LDFLAGS"
		WX_OLD_CPPFLAGS="$CPPFLAGS"
	
		if test "$BUILD_DEBUG" == yes
		then
			WX_NEW_CPPFLAGS=`${WX_CONFIG} --cppflags --unicode=yes --debug=yes --version=${WX_VERSION}`
			CPPFLAGS="$CPPFLAGS $WX_NEW_CPPFLAGS -g -O0"
			
			WX_NEW_LIBS=`${WX_CONFIG} ${WX_STATIC} --libs --unicode=yes --debug=yes --version=${WX_VERSION}`
			WX_NEW_CONTRIB_LIBS=`${WX_CONFIG} ${WX_STATIC} --libs stc,ogl --unicode=yes --debug=yes --version=${WX_VERSION}`
			LIBS="$LIBS $WX_NEW_LIBS $WX_NEW_CONTRIB_LIBS"
		else
			WX_NEW_CPPFLAGS=`${WX_CONFIG} --cppflags --unicode=yes --debug=no --version=${WX_VERSION}`
			CPPFLAGS="$CPPFLAGS $WX_NEW_CPPFLAGS -O2"
		
			WX_NEW_LIBS=`${WX_CONFIG} ${WX_STATIC} --libs --unicode=yes --debug=no --version=${WX_VERSION}`
			WX_NEW_CONTRIB_LIBS=`${WX_CONFIG} ${WX_STATIC} --libs stc,ogl --unicode=yes --debug=no --version=${WX_VERSION}`
			LIBS="$LIBS $WX_NEW_LIBS $WX_NEW_CONTRIB_LIBS"
		fi

		AC_MSG_CHECKING(wxWidgets in ${WX_HOME})
		if test "$WX_NEW_CPPFLAGS" = "" -o "$WX_NEW_LIBS" = "" -o "$WX_NEW_CONTRIB_LIBS" = ""
		then
		    AC_MSG_RESULT(failed)
			AC_MSG_ERROR([Your wxWidgets installation cannot support pgAdmin in the selected configuration. This may be because it was configured without the --enable-unicode option, or the combination of dynamic/static linking and debug/non-debug libraries selected did not match any installed wxWidgets libraries.])
		else
			AC_MSG_RESULT(ok)
		fi

		case "${host}" in
			*-apple-darwin*)
				CPPFLAGS="$CPPFLAGS -no-cpp-precomp -fno-rtti"
				;;
			*solaris*)
				LDFLAGS="$LDFLAGS -lnsl"
				;;
			*)
				;;
		esac
	fi
])
AC_SUBST(WX_CONFIG)

###########
# Cleanup #
###########
AC_DEFUN([CLEANUP],
[
	# CFLAGS/CXXFLAGS may well contain unwanted settings, so clear them.
	CFLAGS=""
	CXXFLAGS=""
])

#########################
# Configuration summary #
#########################
AC_DEFUN([SUMMARY],
[
	# Print a configuration summary
	echo
	echo "PostgreSQL directory:                $PG_HOME"
	echo "PostgreSQL pg_config binary:         $PG_CONFIG"
	echo "PostgreSQL version:                  $PG_VERSION"
	echo
	echo "wxWidgets directory:                 $WX_HOME"
	echo "wxWidgets wx-config binary:          $WX_CONFIG"
	echo "wxWidgets version:                   wxWidgets "`$WX_CONFIG --version --version=$WX_VERSION`
	echo
	if test "$BUILD_DEBUG" == yes
	then
		echo "Building a debug version of pgAdmin: Yes"
	else
		echo "Building a debug version of pgAdmin: No"
	fi
	if test "$BUILD_STATIC" == yes
	then
		echo "Statically linking pgAdmin:          Yes"
	else
		echo "Statically linking pgAdmin:          No"
	fi
	if test "$BUILD_APPBUNDLE" == yes
	then
		echo "Building a Mac OS X appbundle:       Yes"
	else
		echo "Building a Mac OS X appbundle:       No"
	fi
	echo

	echo "pgAdmin configuration is now complete. You can now compile and"
	echo "install pgAdmin using 'make; make install'."
        echo
])
