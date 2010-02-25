#######################################################################
#
# pgAdmin3 - PostgreSQL tools
# Copyright (C) 2002 - 2010, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# $Id$
#
# CMakeLists.txt - CMake build configuration
#
#######################################################################

################################################################################
# Initial setup
################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

IF(COMMAND cmake_policy)
    CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

SET(CMAKE_FIND_LIBRARY_PREFIXES "")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")

SET(STATIC_BUILD YES CACHE BOOL "Statically link the executable?")

################################################################################
# Apple stuff
################################################################################
IF(APPLE)
    # Setup default values
    IF(NOT HAVE_CACHED_VALUES)
        IF(EXISTS /Developer/SDKs/MacOSX10.5.sdk)
            SET(CMAKE_OSX_ARCHITECTURES "ppc;i386" CACHE STRING "Build architectures for OSX" FORCE)
            SET(CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX10.5.sdk" CACHE FILEPATH "isysroot used for universal binary support" FORCE)
        ELSE()
            IF(EXISTS /Developer/SDKs/MacOSX10.4u.sdk)
                SET(CMAKE_OSX_ARCHITECTURES "ppc;i386" CACHE STRING "Build architectures for OSX" FORCE)
                SET(CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX10.4u.sdk" CACHE FILEPATH "isysroot used for universal binary support" FORCE)
            ELSE()
                MESSAGE(FATAL_ERROR "No supported SDK could be found!")
            ENDIF(EXISTS /Developer/SDKs/MacOSX10.4u.sdk)
        ENDIF(EXISTS /Developer/SDKs/MacOSX10.5.sdk)
    ENDIF(NOT HAVE_CACHED_VALUES)

    # Target Tiger
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=10.4")
ENDIF(APPLE)

################################################################################
# Project config
################################################################################
PROJECT(pgadmin3)

SET(VERSION 1.9.0)

# CPack stuff
SET(CPACK_PACKAGE_VERSION_MAJOR 1)
SET(CPACK_PACKAGE_VERSION_MINOR 9)
SET(CPACK_PACKAGE_VERSION_PATCH 0)
SET(CPACK_PACKAGE_NAME "pgAdmin3")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "pgAdmin III is a graphical management tool for PostgreSQL")
SET(CPACK_PACKAGE_VENDOR "The pgAdmin Development Team")
SET(CPACK_GENERATOR TGZ)
SET(CPACK_SOURCE_GENERATOR TGZ)
SET(CPACK_SOURCE_IGNORE_FILES "\\\\.DS_Store;/\\\\.svn/;\\\\.swp$;\\\\.#;/#;.*~;cscope.*")

# This must come after we set the CPACK variables!!
INCLUDE(CPack)

################################################################################
# Find wxWidgets
################################################################################
SET(WX_DEBUG NO CACHE BOOL "Use the debug build of wxWidgets?")
SET(WX_STATIC ${STATIC_BUILD})
SET(WX_VERSION "2.8")
SET(WX_UNICODE YES)
SET(WX_MODULES "base;core;std;stc;xml;adv;ogl;aui;xrc;html;net")
IF(NOT WX_DEBUG)
   ADD_DEFINITIONS("-DEMBED_XRC")
ENDIF(NOT WX_DEBUG)

FIND_PACKAGE(WX REQUIRED)

INCLUDE_DIRECTORIES(${WX_INCLUDE_DIRS})
ADD_DEFINITIONS(${WX_DEFINITIONS})
LINK_DIRECTORIES(${WX_LIBRARY_DIRS})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WX_CXX_FLAGS}")

################################################################################
# Find PostgreSQL
################################################################################
SET(PG_STATIC ${WX_STATIC})

FIND_PACKAGE(PG REQUIRED)

INCLUDE_DIRECTORIES(${PG_INCLUDE_DIRS})
LINK_DIRECTORIES(${PG_LIBRARY_DIRS})

################################################################################
# Find LibXML and LibXSLT
################################################################################
FIND_PACKAGE(LibXml2 REQUIRED)
FIND_PACKAGE(LibXslt REQUIRED)

INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${LIBXSLT_INCLUDE_DIR})

################################################################################
# Check for functions and include files
################################################################################
INCLUDE(CheckFunctionExists)
INCLUDE(CheckIncludeFile)
check_include_file(arpa/inet.h HAVE_ARPA_INET_H)
check_function_exists(gethostbyname HAVE_GETHOSTBYNAME)
check_function_exists(inet_ntoa HAVE_INET_NTOA)
check_include_file(inttypes.h HAVE_INTTYPES_H)
check_function_exists(memmove HAVE_MEMMOVE)
check_include_file(memory.h HAVE_MEMORY_H)
check_function_exists(memset HAVE_MEMSET)
check_include_file(netdb.h HAVE_NETDB_H)
check_include_file(netinet/in.h HAVE_NETINET_IN_H)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(strings.h HAVE_STRINGS_H)
check_include_file(string.h HAVE_STRING_H)
check_include_file(sys/stat.h HAVE_SYS_STAT_H)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(unistd.h HAVE_UNISTD_H)
CONFIGURE_FILE(${pgadmin3_SOURCE_DIR}/config.h.in.cmake ${pgadmin3_SOURCE_DIR}/config.h)
INCLUDE_DIRECTORIES(BEFORE ${pgadmin3_SOURCE_DIR})

################################################################################
# Set global compiler flags
################################################################################
IF (WIN32)
ELSE (WIN32)
  SET(EXTRA_COMPILE_FLAGS "-Wall -Wno-non-virtual-dtor -fno-strict-aliasing")
ENDIF (WIN32)

################################################################################
# Platform specific includes and libs
################################################################################
IF (WIN32)
  # Locate iconv, which is needed by libxml2, but not picked up automatically
  FIND_PATH(ICONV_H_NAME iconv.h REQUIRED)
  INCLUDE_DIRECTORIES(${ICONV_H_NAME})
ENDIF(WIN32)

################################################################################
# Step into subdirs
################################################################################
ADD_SUBDIRECTORY(branding)
ADD_SUBDIRECTORY(docs)
ADD_SUBDIRECTORY(pgadmin)
ADD_SUBDIRECTORY(xtra)

################################################################################
# Installation rules for extra files (subdirs take care of main targets)
################################################################################
INSTALL(FILES ${pgadmin3_SOURCE_DIR}/BUGS DESTINATION share/pgadmin3)
INSTALL(FILES ${pgadmin3_SOURCE_DIR}/CHANGELOG DESTINATION share/pgadmin3)
INSTALL(FILES ${pgadmin3_SOURCE_DIR}/LICENSE DESTINATION share/pgadmin3)

################################################################################
# Build summary
################################################################################
MESSAGE(STATUS " ")
MESSAGE(STATUS "================================================================================")
MESSAGE(STATUS "Configuration summary:")
MESSAGE(STATUS " ")
MESSAGE(STATUS "  Project                     : ${PROJECT_NAME}")
MESSAGE(STATUS "  Description                 : ${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
MESSAGE(STATUS "  Version                     : ${VERSION}")
MESSAGE(STATUS " ")
MESSAGE(STATUS "  Target directory            : ${CMAKE_INSTALL_PREFIX}")
MESSAGE(STATUS " ")
MESSAGE(STATUS "  PostgreSQL version          : ${PG_VERSION_STRING}")
MESSAGE(STATUS "  PostgreSQL path             : ${PG_ROOT_DIR}")
MESSAGE(STATUS "  PostgreSQL config binary    : ${PG_CONFIG_PATH}")
MESSAGE(STATUS "  PostgreSQL include path     : ${PG_INCLUDE_DIRS}")
MESSAGE(STATUS "  PostgreSQL library path     : ${PG_LIBRARY_DIRS}")
MESSAGE(STATUS " ")
MESSAGE(STATUS "  libxml2 include path        : ${LIBXML2_INCLUDE_DIR}")
MESSAGE(STATUS "  libxml2 libraries           : ${LIBXML2_LIBRARIES}")
MESSAGE(STATUS "  libxslt include path        : ${LIBXSLT_INCLUDE_DIR}")
MESSAGE(STATUS "  libxslt libraries           : ${LIBXSLT_LIBRARIES}")
MESSAGE(STATUS " ")
MESSAGE(STATUS "  wxWidgets version           : ${WX_VERSION_STRING}")
MESSAGE(STATUS "  wxWidgets path              : ${WX_ROOT_DIR}")
MESSAGE(STATUS "  wxWidgets config binary     : ${WX_CONFIG_PATH}")
MESSAGE(STATUS "  wxWidgets Static linking    : ${WX_STATIC}")
MESSAGE(STATUS "  wxWidgets Debug?            : ${WX_DEBUG}")
MESSAGE(STATUS "================================================================================")
MESSAGE(STATUS " ")

################################################################################
# Give ourselves a hint that we have cached values - must be last!
################################################################################
IF(NOT HAVE_CACHED_VALUES)
    SET(HAVE_CACHED_VALUES 1 CACHE INTERNAL "Flag to indicate that we have cached values")
ENDIF(NOT HAVE_CACHED_VALUES)

