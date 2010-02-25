#######################################################################
#
# FindPg.cmake - A CMake module for locating PostgreSQL
#
# Dave Page, EnterpriseDB UK Ltd.
# This code is released under the PostgreSQL Licence
#
#######################################################################

# To use this module, simply include it in your CMake project.
# If set, PostgreSQL will be assumed to be in the location specified
# by the PGDIR environment variable. Otherwise, it will be searched
# for in a number of standard locations.
#
# For statically linked builds, the PG_STATIC variable can be set to
# true.
#
# The following CMake variable will be set:
#
# PG_FOUND - Set to TRUE if PostgreSQL is located
# PG_CONFIG_PATH - The pg_config executable path
# PG_ROOT_DIR - The base install directory for PostgreSQL
# PG_INCLUDE_DIRS - The directory containing the PostgreSQL headers.
# PG_LIBRARIES - The PostgreSQL client libraries.
# PG_LIBRARY_DIRS - The directory containing the PostgreSQL client libraries.
# PG_PKG_LIBRARY_DIRS - The directory containing the PostgreSQL package libraries.
# PG_VERSION_STRING - The PostgreSQL version number.

IF(NOT PG_STATIC OR PG_STATIC STREQUAL "")
    SET(_static "no")
ELSE(NOT PG_STATIC OR PG_STATIC STREQUAL "")
    IF(PG_STATIC)
        SET(_static "yes")
    ELSE(PG_STATIC)
        SET(_static "no")
    ENDIF(PG_STATIC)
ENDIF(NOT PG_STATIC OR PG_STATIC STREQUAL "")

IF(NOT $ENV{PGDIR} STREQUAL "")
    FIND_PROGRAM(PG_CONFIG_PATH pg_config 
                 PATH $ENV{PGDIR}/bin
                 DOC "Path to the pg_config executable"
                 NO_DEFAULT_PATH)
ELSE(NOT $ENV{PGDIR} STREQUAL "")
    FIND_PROGRAM(PG_CONFIG_PATH pg_config
                 PATH /usr/local/pgsql/bin
                      /opt/PostgreSQL/*/bin
                      /Library/PostgreSQL/*/bin
                      $ENV{ProgramFiles}/PostgreSQL/*/bin
                      $ENV{SystemDrive}/PostgreSQL/*/bin
                 DOC "Path to the pg_config executable")

ENDIF(NOT $ENV{PGDIR} STREQUAL "")

EXEC_PROGRAM(${PG_CONFIG_PATH} ARGS --version OUTPUT_VARIABLE PG_VERSION_STRING RETURN_VALUE _retval)

IF(NOT _retval)

    SET(PG_FOUND TRUE)

    # Strip the bin and pg_config from the path
    GET_FILENAME_COMPONENT(PG_ROOT_DIR ${PG_CONFIG_PATH} PATH)
    GET_FILENAME_COMPONENT(PG_ROOT_DIR ${PG_ROOT_DIR} PATH)

    IF(WIN32 AND NOT CYGWIN AND NOT MSYS)

        SET(PG_INCLUDE_DIRS "${PG_ROOT_DIR}/include")
        SET(PG_LIBRARY_DIRS "${PG_ROOT_DIR}/lib")
        SET(PG_PKG_LIBRARY_DIRS "${PG_ROOT_DIR}/lib")

        # There iare no static libraries on VC++ builds of PG.
        LIST(APPEND PG_LIBRARIES libpq.lib)

    ELSE(WIN32 AND NOT CYGWIN AND NOT MSYS)

        EXEC_PROGRAM(${PG_CONFIG_PATH} ARGS --includedir OUTPUT_VARIABLE PG_INCLUDE_DIRS)
        EXEC_PROGRAM(${PG_CONFIG_PATH} ARGS --libdir OUTPUT_VARIABLE PG_LIBRARY_DIRS)
        EXEC_PROGRAM(${PG_CONFIG_PATH} ARGS --pkglibdir OUTPUT_VARIABLE PG_PKG_LIBRARY_DIRS)

        IF(_static)
            LIST(APPEND PG_LIBRARIES ${PG_LIBRARY_DIRS}/libpq.a)

            # Check for SSL and Kerberos
            EXEC_PROGRAM("nm" ARGS ${PG_LIBRARY_DIRS}/libpq.a  OUTPUT_VARIABLE _op)

            IF(_op MATCHES "SSL_connect")
                LIST(APPEND PG_LIBRARIES "ssl")
            ENDIF(_op MATCHES "SSL_connect")

            IF(_op MATCHES "krb5_free_principal")
                LIST(APPEND PG_LIBRARIES "krb5")
            ENDIF(_op MATCHES "krb5_free_principal")

            LIST(APPEND PG_LIBRARIES "crypto")

            IF(NOT APPLE)
                LIST(APPEND PG_LIBRARIES "crypt")
            ENDIF(NOT APPLE)
 
        ELSE(_static)
            LIST(APPEND PG_LIBRARIES pq)
        ENDIF(_static)
    ENDIF(WIN32 AND NOT CYGWIN AND NOT MSYS)

ELSE(NOT _retval)
    SET(PG_FOUND FALSE)
    SET(PG_ROOT_DIR PG_ROOT_DIR-NO_FOUND)
    IF(PG_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "No PostgreSQL installation could be found.")
    ELSE(PG_FIND_REQUIRED)
        MESSAGE(STATUS "No PostgreSQL installation could be found.")
    ENDIF(PG_FIND_REQUIRED)
ENDIF(NOT _retval)

