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
	if test "$ac_cv_prog_cxx_g" = no; then
		AC_MSG_ERROR([could not find a suitable C++ compiler to build pgAdmin])
	fi
])

###########################################
# Check if we can use precompiled headers #
###########################################

AC_DEFUN([CHECK_PRECOMP_HEADERS],
[
	AC_MSG_CHECKING([whether to use precompiled headers])
	USE_PRECOMP=""
	AC_ARG_ENABLE(precomp, [  --enable-precomp        Use precompiled headers], USE_PRECOMP="$enableval")

	if test -z "$USE_PRECOMP"; then
		if test "X$GCC" = Xyes; then
			if gcc_version=`$CC -dumpversion` > /dev/null 2>&1; then
				major=`echo $gcc_version | cut -d. -f1`
				minor=`echo $gcc_version | sed "s/@<:@-,a-z,A-Z@:>@.*//" | cut -d. -f2`
				if test -z "$major" || test -z "$minor"; then
					USE_PRECOMP=no
				elif test "$major" -ge 4; then
					USE_PRECOMP=yes
				elif test "$major" -ge 3 && test "$minor" -ge 4; then
					USE_PRECOMP=yes
				else
					USE_PRECOMP=no
				fi
			else
				USE_PRECOMP=no
			fi
		else
			USE_PRECOMP=no
		fi
	fi
	if test "x$USE_PRECOMP" = "xyes"; then
		AC_MSG_RESULT([yes])
	else
		AC_MSG_RESULT([no])
	fi
])
AC_SUBST(USE_PRECOMP)

#################################
# Check this is SUN compiler #
#################################

AC_DEFUN([CHECK_SUN_COMPILER],
[
       $CC -V 2>test.txt
       SUN_STR=`head -1 test.txt |cut -f2 -d" "`
       rm -rf test.txt
       if test "$SUN_STR" = "Sun"; then
               SUN_CC_COMPILER=yes 
       fi

       $CXX -V 2>test.txt
       SUN_STR=`head -1 test.txt |cut -f2 -d" "`
       rm -rf test.txt
       if test "$SUN_STR" = "Sun"; then
               SUN_CXX_COMPILER=yes
       fi

])

#############################
# Override wxWidgets version #
#############################
AC_DEFUN([SET_WX_VERSION],
[
	AC_ARG_WITH(wx-version, [  --with-wx-version=<version number>  the wxWidgets version in major.minor format (default: 2.8)],
	[
		if test "$withval" = yes; then
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
		WX_VERSION="2.8"
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

#########################
# Check libxml2 version #
#########################
AC_DEFUN([CHECK_LIBXML2],
[
	AC_MSG_CHECKING(libxml2 version)
	XML2_VERSION=`${XML2_CONFIG} --version`
	changequote(<<. >>)dnl
	XML2_MAJOR=`expr ${XML2_VERSION} : '\([0-9]*\)'`
	XML2_MINOR=`expr ${XML2_VERSION} : '[0-9]*\.\([0-9]*\)'`
	XML2_REVISION=`expr ${XML2_VERSION} : '[0-9]*\.[0-9]*\.\([0-9]*\)'`
	changequote([, ])dnl
	if test "$XML2_MAJOR" -lt 2; then
		AC_MSG_ERROR([libxml2 2.6.18 or newer is require to build pgAdmin])
	fi
	if test "$XML2_MAJOR" -eq 2 && test "$XML2_MINOR" -lt 6; then
		AC_MSG_ERROR([libxml2 2.6.18 or newer is require to build pgAdmin])
	fi
	if test "$XML2_MAJOR" -eq 2 && test "$XML2_MINOR" -eq 6 && test "$XML2_REVISION" -lt 18; then
		AC_MSG_ERROR([libxml2 2.6.18 or newer is require to build pgAdmin])
	fi
	AC_MSG_RESULT(ok)
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

		if test "$build_cpu-$build_vendor" = "powerpc-apple" -o "$build_cpu-$build_vendor" = "i686-apple"
		then
			echo -n "checking if libpq links against libssl: "
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

		if test "$build_cpu-$build_vendor" = "powerpc-apple"
		then
			echo -n "checking if libpq links against libkrb5: "
			if test "$(otool -L ${PG_HOME}/lib/libpq.?.dylib | grep -c libkrb5)" -gt 0
			then
				PG_KRB5="yes"
			else
				PG_KRB5="no"
			fi
			echo $PG_KRB5
		else
			AC_CHECK_LIB(pq, krb5_free_principal, [PG_KRB5=yes], [PG_KRB5=no])
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

			if test "$PG_KRB5" = "yes"
			then
				LIBS="$LIBS -lkrb5"
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
	
		if test "$BUILD_DEBUG" = yes
		then
			WX_NEW_CPPFLAGS=`${WX_CONFIG} --cppflags --unicode=yes --debug=yes --version=${WX_VERSION} 2> /dev/null`
			CPPFLAGS="$CPPFLAGS $WX_NEW_CPPFLAGS -g -O0"
			
			debugger_LDADD=`${WX_CONFIG} ${WX_STATIC} --libs std,stc,ogl --unicode=yes --debug=yes --version=${WX_VERSION} 2> /dev/null`
			pgadmin3_LDADD=`${WX_CONFIG} ${WX_STATIC} --libs std,stc,ogl --unicode=yes --debug=yes --version=${WX_VERSION} 2> /dev/null`
			pgagent_LDADD=`${WX_CONFIG} ${WX_STATIC} --libs base --unicode=yes --debug=yes --version=${WX_VERSION} 2> /dev/null`
		else
			WX_NEW_CPPFLAGS=`${WX_CONFIG} --cppflags --unicode=yes --debug=no --version=${WX_VERSION} 2> /dev/null`
			CPPFLAGS="$CPPFLAGS $WX_NEW_CPPFLAGS -O2"
		
			debugger_LDADD=`${WX_CONFIG} ${WX_STATIC} --libs std,stc,ogl --unicode=yes --debug=no --version=${WX_VERSION} 2> /dev/null`
			pgadmin3_LDADD=`${WX_CONFIG} ${WX_STATIC} --libs std,stc,ogl --unicode=yes --debug=no --version=${WX_VERSION} 2> /dev/null`
			pgagent_LDADD=`${WX_CONFIG} ${WX_STATIC} --libs base --unicode=yes --debug=no --version=${WX_VERSION} 2> /dev/null`
		fi

		AC_MSG_CHECKING(wxWidgets in ${WX_HOME})
		if test "$WX_NEW_CPPFLAGS" = "" -o "$pgadmin3_LDADD" = ""
		then
			AC_MSG_RESULT(failed)
			AC_MSG_ERROR([Your wxWidgets installation cannot support pgAdmin in the selected configuration. This may be because it was configured without the --enable-unicode option, or the combination of dynamic/static linking and debug/non-debug libraries selected did not match any installed wxWidgets libraries.])
		else
			AC_MSG_RESULT(ok)
		fi

		case "${host}" in
			*-apple-darwin*)
				MAC_PPC=`${WX_CONFIG} --libs | grep -c "arch ppc"`
				MAC_I386=`${WX_CONFIG} --libs | grep -c "arch i386"`
				CPPFLAGS="$CPPFLAGS -no-cpp-precomp -fno-rtti"
                                LDFLAGS="$LDFLAGS -headerpad_max_install_names"
				if test "$MAC_PPC" != "0"
				then
					CPPFLAGS="$CPPFLAGS -arch ppc"
				fi
                                if test "$MAC_I386" != "0"
                                then
                                        CPPFLAGS="$CPPFLAGS -arch i386"
                                fi
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
AC_SUBST(debugger_LDADD)
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
AC_SUBST(pgadmin3_LDADD)
	
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
AC_SUBST(pgadmin3_LDADD)

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
        if test "$PG_SSL" = yes
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
	if test "$BUILD_DEBUG" = yes
	then
		echo "Building a debug version of pgAdmin:	Yes"
	else
		echo "Building a debug version of pgAdmin:	No"
	fi
	if test "$BUILD_STATIC" = yes
	then
		echo "Statically linking pgAdmin:		Yes"
	else
		echo "Statically linking pgAdmin:		No"
	fi
	if test "$BUILD_APPBUNDLE" = yes
	then
		echo "Building a Mac OS X appbundle:		Yes"
	else
		echo "Building a Mac OS X appbundle:		No"
	fi
	if test "$INSTALL_DOCS" = yes
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
