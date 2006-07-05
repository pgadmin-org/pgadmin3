#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2006, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# acinclude.m4 - Build system scripts.
#
#######################################################################

#################################
# Check there is a C++ compiler #
#################################

AC_DEFUN([CHECK_CPP_COMPILER],
[
	if test "$ac_cv_prog_cxx_g" == no; then
		AC_MSG_ERROR([could not find a suitable C++ compiler to build pgAdmin])
	fi
])

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
	AC_ARG_WITH(wx, [  --with-wx=DIR	   root directory for wxWidgets installation],
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

####################
# Locate wxAUI #
####################
AC_DEFUN([LOCATE_WXAUI],
[
	AC_ARG_WITH(wxaui, [  --with-wxaui=DIR	   wxAUI source directory],
	[
		if test "$withval" != no
		then
			WXAUI_HOME="$withval"
						if test ! -f "${WXAUI_HOME}/src/manager.cpp"
						then
								AC_MSG_ERROR([Could not find your wxAUI source code in ${WXAUI_HOME}])
						fi

		fi
	],
	[
		WXAUI_HOME=/usr/local/wxaui
		if test ! -f "${WXAUI_HOME}/src/manager.cpp"
		then
			WXAUI_HOME=/usr/local/src/wxaui
			if test ! -f "${WXAUI_HOME}/src/manager.cpp"
			then
				WXAUI_HOME=/usr/local/src/wxaui-0.9.2
				if test ! -f "${WXAUI_HOME}/src/manager.cpp"
				then
					AC_MSG_ERROR([Could not find your wxAUI source code. You might need to use the --with-wxaui=DIR configure option])
				fi
			fi
		fi
	])
])
AC_SUBST(WXAUI_HOME)

#####################
# Locate libxml	#
#####################
AC_DEFUN([LOCATE_LIBXML2],
[
	AC_ARG_WITH(libxml2, [  --with-libxml2=DIR  root directory for libxml2 installation],
	[
	  if test "$withval" != no
	  then
		 XML2_HOME="$withval"
		 if test ! -f "${XML2_HOME}/bin/xml2-config"
		 then
			AC_MSG_ERROR([Could not find your libxml2 installation in ${XML2_HOME}])
		 fi
	  fi
	  XML2_CONFIG=${XML2_HOME}/bin/xml2-config
   ],
   [
	  XML2_HOME=/usr/local
	  if test ! -f "${XML2_HOME}/bin/xml2-config"
	  then

		  XML2_HOME=/usr
		  if test ! -f "${XML2_HOME}/bin/xml2-config"
		  then
			  XML2_HOME=/mingw
			  if test ! -f "${XML2_HOME}/bin/xml2-config"
			  then
				  AC_MSG_ERROR([Could not find your libxml2 installation. You might need to use the --with-libxml2=DIR configure option])
			  fi
		  fi
	  fi
	  XML2_CONFIG=${XML2_HOME}/bin/xml2-config
   ])
])

#####################
# Locate libxslt    #
#####################
AC_DEFUN([LOCATE_LIBXSLT],
[
	AC_ARG_WITH(libxslt, [  --with-libxslt=DIR  root directory for libxslt installation],
	[
	  if test "$withval" != no
	  then
		 XSLT_HOME="$withval"
		 if test ! -f "${XSLT_HOME}/bin/xslt-config"
		 then
			AC_MSG_ERROR([Could not find your libxslt installation in ${XSLT_HOME}])
		 fi
	  fi
	  XSLT_CONFIG=${XSLT_HOME}/bin/xslt-config
   ],
   [
	  XSLT_HOME=/usr/local
	  if test ! -f "${XSLT_HOME}/bin/xslt-config"
	  then

		  XSLT_HOME=/usr
		  if test ! -f "${XSLT_HOME}/bin/xslt-config"
		  then
			  XSLT_HOME=/mingw
			  if test ! -f "${XSLT_HOME}/bin/xslt-config"
			  then
				  AC_MSG_ERROR([Could not find your libxslt installation. You might need to use the --with-libxslt=DIR configure option])
			  fi
		  fi
	  fi
	  XSLT_CONFIG=${XSLT_HOME}/bin/xslt-config
   ])
])

#####################
# Locate PostgreSQL #
#####################
AC_DEFUN([LOCATE_POSTGRESQL],
[
	AC_ARG_WITH(pgsql, [  --with-pgsql=DIR	root directory for PostgreSQL installation],
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
	AC_ARG_ENABLE(debug, [  --enable-debug	   build a debug version of pgAdmin3],
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
	AC_ARG_ENABLE(static, [  --enable-static	  build a statically linked version of pgAdmin3],
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

#################################################################################
# Check whether to skip installation of documentation provided with pgAdmin III #
#################################################################################
AC_DEFUN([DISABLE_DOCS],
[
	AC_ARG_ENABLE(docs, [  --disable-docs        disable installation of documentation],
	[
		if test "$enableval" = yes
		then
			INSTALL_DOCS=yes
		else
			INSTALL_DOCS=no
		fi
	],
	[
		INSTALL_DOCS=yes
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

		if test "$build_os" = "mingw32"
		then
			CRYPTO_LIB=""
		else
			CRYPTO_LIB="-lcrypto"
		fi
		
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
				LIBS="${PG_LIB}/libpq.a $CRYPT_LIB $LIBS -lssl $CRYPTO_LIB"
			else
				LIBS="${PG_LIB}/libpq.a $CRYPT_LIB $LIBS $CRYPTO_LIB"
			fi
		else
			if test "$PG_SSL" = "yes"
			then
				LIBS="$LIBS -L${PG_LIB} -lpq"
			else
				LIBS="$LIBS -L${PG_LIB} $CRYPTO_LIB -lpq"
			fi
		fi

		AC_LANG_SAVE
		AC_LANG_C
		AC_CHECK_HEADER(libpq-fe.h, [PG_LIBPQFE=yes], [PG_LIBPQFE=no])
		AC_LANG_RESTORE

		if test "$PG_LIBPQ" = "yes"
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
# Check for wxWidgets libraries and headers	#
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
			pgadmin3_LDADD="$WX_NEW_LIBS $WX_NEW_CONTRIB_LIBS"
			pgagent_LDADD=`${WX_CONFIG} ${WX_STATIC} --libs base --unicode=yes --debug=yes --version=${WX_VERSION}`
		else
			WX_NEW_CPPFLAGS=`${WX_CONFIG} --cppflags --unicode=yes --debug=no --version=${WX_VERSION}`
			CPPFLAGS="$CPPFLAGS $WX_NEW_CPPFLAGS -O2"
		
			WX_NEW_LIBS=`${WX_CONFIG} ${WX_STATIC} --libs --unicode=yes --debug=no --version=${WX_VERSION}`
			WX_NEW_CONTRIB_LIBS=`${WX_CONFIG} ${WX_STATIC} --libs stc,ogl --unicode=yes --debug=no --version=${WX_VERSION}`
			pgadmin3_LDADD="$WX_NEW_LIBS $WX_NEW_CONTRIB_LIBS"
			pgagent_LDADD=`${WX_CONFIG} ${WX_STATIC} --libs base --unicode=yes --debug=no --version=${WX_VERSION}`
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
AC_SUBST(pgadmin3_LDADD)
AC_SUBST(pgagent_LDADD)

#########################
# Setup libxml2 headers #
#########################
AC_DEFUN([SETUP_LIBXML2],
[
	if test -n "${XML2_HOME}"
	then
		XML2_CFLAGS=`${XML2_CONFIG} --cflags`
		XML2_LIBS=`${XML2_CONFIG} --libs`
		AC_MSG_CHECKING(libxml2 in ${XML2_HOME})
		if test "${XML2_CFLAGS}" = "" -o "${XML2_LIBS}" = ""
		then
			AC_MSG_RESULT(failed)
			AC_MSG_ERROR([Your libxml2 installation does not appear to be complete])
		else
			AC_MSG_RESULT(ok)
			CPPFLAGS="$CPPFLAGS $XML2_CFLAGS"
			pgadmin3_LDADD="${pgadmin3_LDADD} $XML2_LIBS"
		fi
	fi
])
AC_SUBST(XML2_CONFIG)
AC_SUBST(pgagent_LDADD)
	
#########################
# Setup libxslt headers #
#########################
AC_DEFUN([SETUP_LIBXSLT],
[
	if test -n "${XSLT_HOME}"
	then
		XSLT_CFLAGS=`${XSLT_CONFIG} --cflags`
		XSLT_LIBS=`${XSLT_CONFIG} --libs`
		AC_MSG_CHECKING(libxslt in ${XSLT_HOME})
		if test "${XSLT_CFLAGS}" = "" -o "${XSLT_LIBS}" = ""
		then
			AC_MSG_RESULT(failed)
			AC_MSG_ERROR([Your libxslt installation does not appear to be complete])
		else
			AC_MSG_RESULT(ok)
			CPPFLAGS="$CPPFLAGS $XSLT_CFLAGS"
			pgadmin3_LDADD="${pgadmin3_LDADD} $XSLT_LIBS"
		fi
	fi
])
AC_SUBST(XSLT_CONFIG)
AC_SUBST(pgagent_LDADD)

#######################
# Setup wxAUI headers #
#######################
AC_DEFUN([SETUP_WXAUI],
[
	CPPFLAGS="$CPPFLAGS -I${WXAUI_HOME}/include"
])

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
	echo "PostgreSQL directory:			$PG_HOME"
	echo "PostgreSQL pg_config binary:		$PG_CONFIG"
	echo "PostgreSQL version:			$PG_VERSION"
        if test "$PG_SSL" == yes
        then
                echo "PostgreSQL SSL support:                 Present"
        else
                echo "PostgreSQL SSL support:                 Missing"
        fi
	echo
	echo "wxWidgets directory:			$WX_HOME"
	echo "wxWidgets wx-config binary:		$WX_CONFIG"
	echo "wxWidgets version:			wxWidgets "`$WX_CONFIG --version --version=$WX_VERSION`
	echo
	echo "wxAUI source directory:			$WXAUI_HOME"
	echo
	if test "$BUILD_DEBUG" == yes
	then
		echo "Building a debug version of pgAdmin:	Yes"
	else
		echo "Building a debug version of pgAdmin:	No"
	fi
	if test "$BUILD_STATIC" == yes
	then
		echo "Statically linking pgAdmin:		Yes"
	else
		echo "Statically linking pgAdmin:		No"
	fi
	if test "$BUILD_APPBUNDLE" == yes
	then
		echo "Building a Mac OS X appbundle:		Yes"
	else
		echo "Building a Mac OS X appbundle:		No"
	fi
	if test "$INSTALL_DOCS" == yes
	then
		echo "Include documentation:			Yes"
	else
		echo "Include documentation:			No"
	fi
	echo

	echo "pgAdmin configuration is now complete. You can now compile and"
	echo "install pgAdmin using 'make; make install'."
	echo
])
