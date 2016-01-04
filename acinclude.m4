#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
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

m4_include([lib-prefix.m4])
m4_include([lib-ld.m4])
m4_include([lib-link.m4])

#############################
# Override wxWidgets version #
#############################
AC_DEFUN([SET_WX_VERSION],
[
	AC_ARG_WITH(wx-version, [  --with-wx-version=<version>	the wxWidgets version in major.minor format (default: 2.8)],
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
	AC_ARG_WITH(wx, [  --with-wx=DIR		root directory for wxWidgets installation],
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
		    # Search the path
				    AC_PATH_PROGS(WX_CONFIG, wx-config)
		    if test ! -f "${WX_CONFIG}"
				    then
			AC_MSG_ERROR([Could not find your wxWidgets installation. You might need to use the --with-wx=DIR configure option])
		    else
					   WX_HOME=`${WX_CONFIG} --prefix`
		    fi
				fi
			fi
		fi
		WX_CONFIG=${WX_HOME}/bin/wx-config
	])
])

###########################
# Check wxWidgets version #
###########################
AC_DEFUN([CHECK_WXWIDGETS],
[
	AC_MSG_CHECKING(wxWidgets version)
	TMP_WX_VERSION=`${WX_CONFIG} --version=${WX_VERSION} --version 2> /dev/null`
	if test "$TMP_WX_VERSION" = ""
	then
		 AC_MSG_RESULT(failed)
		 AC_MSG_ERROR([The version of wxWidgets required (${WX_VERSION}) is not supported by the installations in ${WX_HOME}.])
	fi
	changequote(<<. >>)dnl
	WX_MAJOR=`expr ${TMP_WX_VERSION} : '\([0-9]*\)'`
	WX_MINOR=`expr ${TMP_WX_VERSION} : '[0-9]*\.\([0-9]*\)'`
	changequote([, ])dnl
	if test "$WX_MAJOR" -lt 2; then
		AC_MSG_ERROR([wxWidgets 2.8.0 or newer is required to build pgAdmin])
	fi
	if test "$WX_MAJOR" -eq 2 && test "$WX_MINOR" -lt 8; then
		AC_MSG_ERROR([wxWidgets 2.8.0 or newer is required to build pgAdmin])
	fi
	AC_MSG_RESULT(ok)
])


#####################
# Locate libxml	#
#####################
AC_DEFUN([LOCATE_LIBXML2],
[
	AC_ARG_WITH(libxml2, [  --with-libxml2=DIR	root directory for libxml2 installation],
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
		  # Search the path
				  AC_PATH_PROGS(XML2_CONFIG, xml2-config)
		  if test ! -f "${XML2_CONFIG}"
				  then
		      AC_MSG_ERROR([Could not find your libxml2 installation. You might need to use the --with-libxml2=DIR configure option])
		  else
					  XML2_HOME=`${XML2_CONFIG} --prefix`
		  fi
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
		AC_MSG_ERROR([libxml2 2.6.18 or newer is required to build pgAdmin])
	fi
	if test "$XML2_MAJOR" -eq 2 && test "$XML2_MINOR" -lt 6; then
		AC_MSG_ERROR([libxml2 2.6.18 or newer is required to build pgAdmin])
	fi
	if test "$XML2_MAJOR" -eq 2 && test "$XML2_MINOR" -eq 6 && test "$XML2_REVISION" -lt 18; then
		AC_MSG_ERROR([libxml2 2.6.18 or newer is required to build pgAdmin])
	fi
	AC_MSG_RESULT(ok)
])


#####################
# Locate libxslt    #
#####################
AC_DEFUN([LOCATE_LIBXSLT],
[
	AC_ARG_WITH(libxslt, [  --with-libxslt=DIR	root directory for libxslt installation],
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
		  # Search the path
				  AC_PATH_PROGS(XSLT_CONFIG, xslt-config)
		  if test ! -f "${XSLT_CONFIG}"
				  then
		      AC_MSG_ERROR([Could not find your libxslt installation. You might need to use the --with-libxslt=DIR configure option])
		  else
					  XSLT_HOME=`${XSLT_CONFIG} --prefix`
		  fi
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
				    # Search the path
					AC_PATH_PROGS(PG_CONFIG, pg_config)
				    if test ! -f "${PG_CONFIG}"
				    then
					    AC_MSG_ERROR([Could not find your PostgreSQL installation. You might need to use the --with-pgsql=DIR configure option])
					else
					    PG_HOME=`${PG_CONFIG} --bindir | sed "s/\/bin$//"`
					fi
				fi
			fi
		fi
		PG_CONFIG=${PG_HOME}/bin/pg_config
	])
])

#################
# Locate Sphinx #
#################
AC_DEFUN([LOCATE_SPHINX],
[
        AC_ARG_WITH(sphinx-build, [  --with-sphinx-build=FILE sphinx-build executable to build the docs with],
        [
                if test "$withval" != no
                then
                        SPHINX_BUILD="$withval"
                        if test ! -f "${SPHINX_BUILD}"
                        then
                                AC_MSG_ERROR([Could not find your sphinx-build executable ${SPHINX_BUILD}])
                        fi

                fi
        ],
        [
                SPHINX_BUILD=/usr/local/bin/sphinx-build
                if test ! -f "${SPHINX_BUILD}"
                then
                        SPHINX_BUILD=/usr/bin/sphinx-build
                        if test ! -f "${SPHINX_BUILD}"
                        then
                                # Search the path
				SPHINX_BUILD=""
                                AC_PATH_PROGS(SPHINX_BUILD, sphinx-build sphinx-build-2.7 sphinx-build-2.6, "")
                                if test ! -f "${SPHINX_BUILD}"
                                then
                                        AC_MSG_WARN([Could not find your sphinx-build executable. You might need to use the --with-sphinx-build=FILE configure option])
                                fi
                        fi
                fi
        ])
])
AC_SUBST(SPHINX_BUILD)

########################################
# Enable Database Designer in pgAdmin3 #
########################################
AC_DEFUN([ENABLE_DATABASEDESIGNER],
[
	AC_ARG_ENABLE(databasedesigner, [  --enable-databasedesigner	build the database designer of pgAdmin3],
	[
		if test "$enableval" = yes
		then
			HAVE_DATABASEDESIGNER=yes
		else
			HAVE_DATABASEDESIGNER=no
		fi
	],
	[
		HAVE_DATABASEDESIGNER=no
	])
])
AC_SUBST(HAVE_DATABASEDESIGNER)

###########################
# Debug build of pgAdmin3 #
###########################
AC_DEFUN([ENABLE_DEBUG],
[
	AC_ARG_ENABLE(debug, [  --enable-debug	build a debug version of pgAdmin3],
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
	AC_ARG_ENABLE(static, [  --enable-static	build a statically linked version of pgAdmin3],
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

#################################
# Override the OSX architecture #
#################################
AC_DEFUN([SETUP_ARCH_I386],
[
	AC_ARG_WITH(arch-i386, [  --with-arch-i386	include an i386 image in an OS X Universal build],
	[
		if test "$withval" = "yes"
		then
			OSX_ARCH="$OSX_ARCH -arch i386"
		fi
	])
])
AC_DEFUN([SETUP_ARCH_X86_64],
[	AC_ARG_WITH(arch-x86_64, [  --with-arch-x86_64	include an x86_64 image in an OS X Universal build],
	[
		if test "$withval" = "yes"
		then
			OSX_ARCH="$OSX_ARCH -arch x86_64"
		fi
	])
])
AC_DEFUN([SETUP_ARCH_PPC],
[	AC_ARG_WITH(arch-ppc, [  --with-arch-ppc	include a PPC image in an OS X Universal build],
	[
		if test "$withval" = "yes"
		then
			OSX_ARCH="$OSX_ARCH -arch ppc"
		fi
	])
])
AC_DEFUN([SETUP_ARCH_PPC64],
[	AC_ARG_WITH(arch-ppc64, [  --with-arch-ppc64	include a PPC64 image in an OS X Universal build],
	[
		if test "$withval" = "yes"
		then
			OSX_ARCH="$OSX_ARCH -arch ppc64"
		fi
	])
])

##########################
# Build a Mac App Bundle #
##########################
AC_DEFUN([ENABLE_APPBUNDLE],
[
	AC_ARG_ENABLE(appbundle, [  --enable-appbundle	Build a Mac OS X appbundle],
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
	    PG_LIB=`${PG_CONFIG} --libdir`
		
		PGSQL_OLD_LDFLAGS="$LDFLAGS"
		PGSQL_OLD_CPPFLAGS="$CPPFLAGS"

		AC_LANG_SAVE    
		AC_LANG_C	       
		AC_CHECK_LIB(ssl, SSL_library_init, [LIB_SSL=yes], [LIB_SSL=no])
		AC_LANG_RESTORE	 

		AC_LANG_SAVE    
		AC_LANG_C	       
		AC_CHECK_LIB(krb5, krb5_sendauth, [LIB_KRB5=yes], [LIB_KRB5=no])
		AC_LANG_RESTORE	 


		# Solaris/Mac need -lssl for this test
		case "${host}" in
			*solaris* | *-apple-darwin*)
		if test "$LIB_SSL" = "yes"
				then
					LDFLAGS="$LDFLAGS -L${PG_LIB} -lssl"
				else
					LDFLAGS="$LDFLAGS -L${PG_LIB}"
				fi
				;;
			*)
				LDFLAGS="$LDFLAGS -L${PG_LIB}"
				;;
		esac

		
		# Check for PQexec (basic sanity check!)
		if test "$BUILD_STATIC" = "yes"
		then
			AC_MSG_CHECKING(for PQexec in libpq.a)
			if test "$(nm ${PG_LIB}/libpq.a | grep -c PQexec)" -gt 0
			then
				AC_MSG_RESULT(present)
				PG_LIBPQ="yes" 
			else    
				AC_MSG_RESULT(not present)
				PG_LIBPQ="no"
			fi	      
		else			    
			AC_LANG_SAVE    
			AC_LANG_C	       
			AC_CHECK_LIB(pq, PQexec, [PG_LIBPQ=yes], [PG_LIBPQ=no])
			AC_LANG_RESTORE	 
		fi

		# Check for PQconninfoParse
		if test "$BUILD_STATIC" = "yes"
		then
			AC_MSG_CHECKING(for PQconninfoParse in libpq.a)
			if test "$(nm ${PG_LIB}/libpq.a | grep -c PQconninfoParse)" -gt 0
			then
				AC_MSG_RESULT(present)
				HAVE_CONNINFO_PARSE="yes"
			else
				AC_MSG_RESULT(not present)
				HAVE_CONNINFO_PARSE="no"
			fi
		else
			AC_LANG_SAVE
			AC_LANG_C
			AC_CHECK_LIB(pq, PQconninfoParse, [HAVE_CONNINFO_PARSE=yes], [HAVE_CONNINFO_PARSE=no])
			AC_LANG_RESTORE
		fi

		AC_LANG_SAVE
		AC_LANG_C

		if test "$LIB_SSL" = "yes"
		then
		# Check for SSL support
		if test "$BUILD_STATIC" = "yes"
		then
			AC_MSG_CHECKING(for SSL_connect in libpq.a)
			if test "$(nm ${PG_LIB}/libpq.a | grep -c SSL_connect)" -gt 0
			then
				AC_MSG_RESULT(present)
				PG_SSL="yes"
			else   
				AC_MSG_RESULT(not present)
				PG_SSL="no"
			fi
		else
			if test "$build_cpu-$build_vendor" = "powerpc-apple" -o "$build_cpu-$build_vendor" = "i386-apple" -o "$build_cpu-$build_vendor" = "i686-apple"
			then
				AC_MSG_CHECKING(for SSL_connect in -lpq)
				if test "$(otool -L ${PG_LIB}/libpq.?.dylib | grep -c libssl)" -gt 0
				then
					AC_MSG_RESULT(present)
					PG_SSL="yes"
				else
					AC_MSG_RESULT(not present)
					PG_SSL="no"
				fi
			else
				AC_CHECK_LIB(pq, SSL_connect, [PG_SSL=yes], [PG_SSL=no], "-lssl")
			fi
		fi
		else
			PG_SSL="no"
		fi

		if test "$LIB_KRB5" = "yes"
		then
		# Check for Kerberos support

				LDFLAGS="$LDFLAGS -lkrb5"

		if test "$BUILD_STATIC" = "yes"
		then
			AC_MSG_CHECKING(for krb5_free_principal in libpq.a)
			if test "$(nm ${PG_LIB}/libpq.a | grep -c krb5_free_principal)" -gt 0
			then
				AC_MSG_RESULT(present)
				PG_KRB5="yes"
			else
				AC_MSG_RESULT(not present)
				PG_KRB5="no"
			fi
		else
			if test "$build_cpu-$build_vendor" = "powerpc-apple" -o "$build_cpu-$build_vendor" = "i386-apple" -o "$build_cpu-$build_vendor" = "i686-apple"
			then
				AC_MSG_CHECKING(for krb5_free_principle in -lpq)
				if test "$(otool -L ${PG_LIB}/libpq.?.dylib | grep -c libkrb5)" -gt 0
				then
					AC_MSG_RESULT(present)
					PG_KRB5="yes"
				else
					AC_MSG_RESULT(not present)
					PG_KRB5="no"
				fi
			else
				AC_CHECK_LIB(pq, krb5_free_principal, [PG_KRB5=yes], [PG_KRB5=no])
			fi
		fi
		else
			PG_KRB5="no"
		fi

		AC_LANG_RESTORE

		PG_INCLUDE=`${PG_CONFIG} --includedir` 
		PG_SVRINCLUDE=`${PG_CONFIG} --includedir-server`
		PG_PKGINCLUDE=`${PG_CONFIG} --pkgincludedir`
		CPPFLAGS="$CPPFLAGS -I${PG_INCLUDE} -I${PG_SVRINCLUDE} -I${PG_PKGINCLUDE}"
	
		PG_VERSION=`${PG_CONFIG} --version`

		if test "$build_os" = "mingw32"
		then
			CRYPTO_LIB=""
		else
			CRYPTO_LIB="-lcrypto"
		fi
		
		if test "$BUILD_STATIC" = "yes"
		then
			if test "$build_cpu-$build_vendor" = "powerpc-apple" -o "$build_cpu-$build_vendor" = "i386-apple" -o "$build_cpu-$build_vendor" = "i686-apple"
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
			AC_MSG_ERROR([you must specify a valid PostgreSQL 8.4+ installation with --with-pgsql=DIR])
		fi

		if test "$PG_SSL" = "yes"
		then
			CPPFLAGS="$CPPFLAGS -DPG_SSL"
		fi
		if test "$HAVE_CONNINFO_PARSE" = "yes"
		then
			CPPFLAGS="$CPPFLAGS -DHAVE_CONNINFO_PARSE"
		fi
		if test "$HAVE_DATABASEDESIGNER" = "yes"
		then
			CPPFLAGS="$CPPFLAGS -DDATABASEDESIGNER"
		fi

		# Avoid linking with things we don't need. Really this is a hack
		# to prevent png2c linking with libpq with gcc on non-OSX OSs
		if test "$SUN_CC_COMPILER" != "yes"
		then
			case "${host}" in
				*-apple-darwin*)
					;;
				*solaris*)
					LDFLAGS="$LDFLAGS"
					;;
				*)
					LDFLAGS="$LDFLAGS -Wl,-as-needed"
					;;
			esac
		fi
	fi
])
AC_SUBST(PG_CONFIG)

#######################################################
# Check for extended libpq functions in EnterpriseDB  #
#######################################################
AC_DEFUN([CHECK_EDB_LIBPQ],
[
	if test "$BUILD_STATIC" = "yes"
	then
		AC_MSG_CHECKING(for PQgetOutResult in libpq.a)
		if test "$(nm ${PG_LIB}/libpq.a | grep -c PQgetOutResult)" -gt 0
		then
			AC_MSG_RESULT(present)
			EDB_LIBPQ="yes"
		else
			AC_MSG_RESULT(not present)
			EDB_LIBPQ="no"
		fi
	else
		AC_LANG_SAVE
		AC_LANG_C
		AC_CHECK_LIB(pq, PQgetOutResult, [EDB_LIBPQ=yes], [EDB_LIBPQ=no])
		AC_LANG_RESTORE
	fi
])
AC_SUBST(EDB_LIBPQ)

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
			CPPFLAGS="$CPPFLAGS $WX_NEW_CPPFLAGS"
			CFLAGS=`echo $CFLAGS | sed -e "s/-O2/-O0/g"`
			CXXFLAGS=`echo $CXXFLAGS | sed -e "s/-O2/-O0/g"`
			
			pgadmin3_LDADD=`${WX_CONFIG} ${WX_STATIC} --libs std,stc,aui --unicode=yes --debug=yes --version=${WX_VERSION} 2> /dev/null`
			pgsTest_LDADD=`${WX_CONFIG} ${WX_STATIC} --libs base,core,xml --unicode=yes --debug=yes --version=${WX_VERSION} 2> /dev/null`
			pgScript_LDADD=`${WX_CONFIG} ${WX_STATIC} --libs base,core,xml --unicode=yes --debug=yes --version=${WX_VERSION} 2> /dev/null`
		else
			WX_NEW_CPPFLAGS=`${WX_CONFIG} --cppflags --unicode=yes --debug=no --version=${WX_VERSION} 2> /dev/null`
			CPPFLAGS="$CPPFLAGS $WX_NEW_CPPFLAGS -DEMBED_XRC"
			CFLAGS=`echo $CFLAGS | sed -e "s/-g //g"`
			CXXFLAGS=`echo $CXXFLAGS | sed -e "s/-g //g"`
		
			pgadmin3_LDADD=`${WX_CONFIG} ${WX_STATIC} --libs std,stc,aui --unicode=yes --debug=no --version=${WX_VERSION} 2> /dev/null`
			pgsTest_LDADD=`${WX_CONFIG} ${WX_STATIC} --libs base,core,xml --unicode=yes --debug=no --version=${WX_VERSION} 2> /dev/null`
			pgScript_LDADD=`${WX_CONFIG} ${WX_STATIC} --libs base,core,xml --unicode=yes --debug=no --version=${WX_VERSION} 2> /dev/null`
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

				# Use the default arch if none is specified.
				if test "$OSX_ARCH" = ""
				then
					OSX_ARCH="-arch `uname -p`"
				fi

				LDFLAGS="$LDFLAGS -headerpad_max_install_names"
				CPPFLAGS="$CPPFLAGS $OSX_ARCH" 

				# Strip any existing arch flags from LDFLAGS and add the desired ones
				# This is required as wxWidgets 2.8 (but not 2.9) includes the arch flags
				# in it's --libs output.
				pgadmin3_LDADD=`echo $pgadmin3_LDADD | sed -e "s/-arch ppc//g" -e "s/-arch i386//g" -e "s/-arch x86_64//g" -e "s/-arch ppc64//g"` 
				pgadmin3_LDADD="$pgadmin3_LDADD $OSX_ARCH"
				pgsTest_LDADD=`echo $pgsTest_LDADD | sed -e "s/-arch ppc//g" -e "s/-arch i386//g" -e "s/-arch x86_64//g" -e "s/-arch ppc64//g"`
				pgsTest_LDADD="$pgsTest_LDADD $OSX_ARCH"
				pgScript_LDADD=`echo $pgScript_LDADD | sed -e "s/-arch ppc//g" -e "s/-arch i386//g" -e "s/-arch x86_64//g" -e "s/-arch ppc64//g"`
				pgScript_LDADD="$pgScript_LDADD $OSX_ARCH"
				png2c_LDADD="$OSX_ARCH"

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
AC_SUBST(pgsTest_LDADD)
AC_SUBST(pgScript_LDADD)
AC_SUBST(png2c_LDADD)

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

#################
# Setup libssh2 #
#################

sinclude(acinclude-ssh2.m4)


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
	echo
	if test "$HAVE_CONNINFO_PARSE" = yes
	then
		echo "PostgreSQL PQconninfoParse support:     Present"
	else
		echo "PostgreSQL PQconninfoParse support:     Missing"
	fi
	if test "$PG_SSL" = yes
	then
		echo "PostgreSQL SSL support:			Present"
	else
		echo "PostgreSQL SSL support:			Missing"
	fi
	echo
	echo "wxWidgets directory:			$WX_HOME"
	echo "wxWidgets wx-config binary:		$WX_CONFIG"
	echo "wxWidgets version:			wxWidgets "`$WX_CONFIG --version --version=$WX_VERSION`
	echo
	echo "libxml2 directory:			$XML2_HOME"
	echo "libxml2 xml2-config binary:		$XML2_CONFIG"
	echo "libxml2 version:			libxml2 "`$XML2_CONFIG --version`
	echo
	echo "libxslt directory:			$XSLT_HOME"
	echo "libxslt xslt-config binary:		$XSLT_CONFIG"
	echo "libxslt version:			libxslt "`$XSLT_CONFIG --version`
	echo
	if test "$HAVE_DATABASEDESIGNER" = yes
	then
		echo "Building Database Designer:		Yes"
	else
		echo "Building Database Designer:		No"
	fi
	echo
	if test "$BUILD_SSH_TUNNEL" = yes
	then
		echo "Building SSH Tunnel:			Yes"
		if test "$ac_cv_libssl" = yes
		then
			echo "Crypto library:				OpenSSL"
		else test "$ac_cv_libgcrypt" = yes
			echo "Crypto library:				libgcrypt"
		fi
		if test "$ac_cv_libz" = yes
		then
			echo "libz compression:			yes"
		else
			echo "libz compression:			no"
		fi
	else
		echo "Building SSH Tunnel:			No"
	fi
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
	echo
        if test "$SPHINX_BUILD" = ""
        then
                echo "sphinx-build executable:                <not found>"
        else
                echo "sphinx-build executable:                ${SPHINX_BUILD}"
        fi
	echo
	echo "pgAdmin configuration is now complete. You can now compile and"
	echo "install pgAdmin using 'make; make install'."
	echo
])
