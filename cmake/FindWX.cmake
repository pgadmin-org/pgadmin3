#######################################################################
#
# FindPg.cmake - A CMake module for locating wxWidgets
#
# Dave Page, EnterpriseDB UK Ltd.
# This code is released under the BSD Licence
#
#######################################################################

# To use this module, simply include it in your CMake project.
# If set, wxWidgets will be assumed to be in the location specified
# by the WXWIN environment variable. Otherwise, it will be searched
# for in a number of standard locations.
#
# The following CMake variables can be set to control the build type.
# If not set, the default values shown will be used. Booleans must 
# be either YES or NO:
#
# WX_VERSION = "2.8"
# WX_DEBUG = NO
# WX_STATIC = NO
# WX_UNICODE = YES
# WX_MODULES = "base" (a list).
#
# The following CMake variable will be set:
#
# WX_FOUND - Set to TRUE if wxWidgets is located
# WX_ROOT_DIR - The base install directory for wxWidgets
# WX_VERSION_STRING - The wxWidgets version number.
#
# Unix only:
# WX_CONFIG_PATH - The wx-config executable path
#
# Unix & Win32:
# WX_INCLUDE_DIRS - The wxWidgets header directories.
# WX_DEFINITIONS - The wxWidgets preprocessor definitions
# WX_LIBRARIES - The wxWidgets libraries
# WX_LIBRARY_DIRS - The wxWidgets library directories.


###############################################################################
# Macros
###############################################################################

# Check for a library on Windows
MACRO(WIN32_CHECK_LIB M_LIB_NAME M_LIB_REL_FILENAME M_LIB_DBG_FILENAME M_LIB_PATH)

    LIST(FIND WX_MODULES ${M_LIB_NAME} _pos)
    IF(NOT _pos EQUAL -1)

        SET(_tmp CACHE INTERNAL "_tmp-NOTFOUND")
        FIND_FILE(_tmp NAMES ${M_LIB_REL_FILENAME} PATHS ${M_LIB_PATH} NO_DEFAULT_PATH)
        IF(_tmp STREQUAL "_tmp-NOTFOUND")
            SET(_error TRUE)
        ELSE(_tmp STREQUAL "_tmp-NOTFOUND")
            LIST(APPEND WX_LIBRARIES "optimized;${M_LIB_REL_FILENAME}")
        ENDIF(_tmp STREQUAL "_tmp-NOTFOUND")
        
        SET(_tmp CACHE INTERNAL "_tmp-NOTFOUND")
        FIND_FILE(_tmp NAMES ${M_LIB_DBG_FILENAME} PATHS ${M_LIB_PATH} NO_DEFAULT_PATH)
        IF(_tmp STREQUAL "_tmp-NOTFOUND")
            SET(_error TRUE)
        ELSE(_tmp STREQUAL "_tmp-NOTFOUND")
            LIST(APPEND WX_LIBRARIES "debug;${M_LIB_DBG_FILENAME}")
        ENDIF(_tmp STREQUAL "_tmp-NOTFOUND")

    ENDIF(NOT _pos EQUAL -1)

ENDMACRO(WIN32_CHECK_LIB)

###############################################################################
# Arguments
###############################################################################

IF(NOT WX_VERSION OR WX_VERSION STREQUAL "")
    SET(_version "2.8")
ELSE(NOT WX_VERSION OR WX_VERSION STREQUAL "")
    SET(_version ${WX_VERSION})
ENDIF(NOT WX_VERSION OR WX_VERSION STREQUAL "")

IF(NOT WX_DEBUG OR WX_DEBUG STREQUAL "")
    SET(_debug "no")
ELSE(NOT WX_DEBUG OR WX_DEBUG STREQUAL "")
    IF(WX_DEBUG)
        SET(_debug "yes")
    ELSE(WX_DEBUG)
        SET(_debug "no")
    ENDIF(WX_DEBUG)
ENDIF(NOT WX_DEBUG OR WX_DEBUG STREQUAL "")

IF(NOT WX_STATIC OR WX_STATIC STREQUAL "")
    SET(_static "no")
ELSE(NOT WX_STATIC OR WX_STATIC STREQUAL "")
    IF(WX_STATIC)
        SET(_static "yes")
    ELSE(WX_STATIC)
        SET(_static "no")
    ENDIF(WX_STATIC)
ENDIF(NOT WX_STATIC OR WX_STATIC STREQUAL "")

IF(NOT WX_UNICODE OR WX_UNICODE STREQUAL "")
    SET(_unicode "yes")
ELSE(NOT WX_UNICODE OR WX_UNICODE STREQUAL "")
    IF(WX_UNICODE)
        SET(_unicode "yes")
    ELSE(WX_UNICODE)
        SET(_unicode "no")
    ENDIF(WX_UNICODE)
ENDIF(NOT WX_UNICODE OR WX_UNICODE STREQUAL "")

IF(NOT WX_MODULES OR WX_MODULES STREQUAL "")
    SET(_modules "base")
ELSE(NOT WX_MODULES OR WX_MODULES STREQUAL "")
    SET(_modules ${WX_MODULES})
ENDIF(NOT WX_MODULES OR WX_MODULES STREQUAL "")

SET(_build_desc "version: ${_version}, debug: ${_debug}, static: ${_static}, unicode: ${_unicode}, modules: ${_modules}")

###############################################################################
# Here we go...
###############################################################################

# MSVC++
IF(WIN32 AND NOT CYGWIN AND NOT MSYS)
    # The VC++ libraries are found in an entirely different way than
    # the *nix libraries because we don't have a wx-config file.
    
    # Figure out the build suffix
    SET(_suffix "")    # Possibly-unicode libraries

    IF(_unicode)
        SET(_suffix "${_suffix}u")
    ENDIF(_unicode)

    # Figure out the build prefix directory
    IF(_static)
        SET(_prefix "vc_lib")
    ELSE(_static)
        SET(_prefix "vc_dll")
    ENDIF(_static)
    
    # Find the Unix configure script. We'll attempt to extract a version number from it.
    IF(NOT $ENV{WXWIN} STREQUAL "")
        FIND_PATH(WX_ROOT_DIR NAMES configure
                  PATHS $ENV{WXWIN}
                  DOC "Path to the wxWidgets installation"
                  NO_DEFAULT_PATH)
    ELSE(NOT $ENV{WXWIN} STREQUAL "")
        FIND_PATH(WX_ROOT_DIR NAMES configure
                  PATHS $ENV{ProgramFiles}/wxWidgets-*
                        $ENV{SystemDrive}/wxWidgets-*
                  DOC "Path to the wxWidgets installation")
    ENDIF(NOT $ENV{WXWIN} STREQUAL "")

    # Attempt to read the version number from the configure script
    FILE(STRINGS ${WX_ROOT_DIR}/configure _line REGEX "PACKAGE_VERSION=")
    STRING(REGEX REPLACE "PACKAGE_VERSION='([0-9]+\\.[0-9]+\\.[0-9]+)'" "\\1" WX_VERSION_STRING "${_line}")

    IF(NOT ${WX_VERSION_STRING} STREQUAL "")

        SET(WX_FOUND TRUE)

        LIST(APPEND WX_INCLUDE_DIRS "${WX_ROOT_DIR}/include" "${WX_ROOT_DIR}/contrib/include")
        LIST(APPEND WX_LIBRARY_DIRS "${WX_ROOT_DIR}/lib/${_prefix}")

        # Got through the modules list and add libraries for those requested.
        # If any of them don't seem to exist, throw an error, or got to not found mode
        STRING(REGEX REPLACE "^([0-9])+\\.([0-9])+\\.[0-9]+" "\\1\\2" _shortver "${WX_VERSION_STRING}")

        SET(_error FALSE)
        
        SET(_libpath ${WX_ROOT_DIR}/lib/${_prefix})

        WIN32_CHECK_LIB("adv" wxmsw${_shortver}${_suffix}_adv.lib wxmsw${_shortver}${_suffix}d_adv.lib ${_libpath})
        WIN32_CHECK_LIB("aui" wxmsw${_shortver}${_suffix}_aui.lib wxmsw${_shortver}${_suffix}d_aui.lib ${_libpath})
        WIN32_CHECK_LIB("base" wxbase${_shortver}${_suffix}.lib wxbase${_shortver}${_suffix}d.lib ${_libpath})
        WIN32_CHECK_LIB("core" wxmsw${_shortver}${_suffix}_core.lib wxmsw${_shortver}${_suffix}d_core.lib ${_libpath})
        WIN32_CHECK_LIB("dbgrid" wxmsw${_shortver}${_suffix}_dbgrid.lib wxmsw${_shortver}${_suffix}d_dbgrid.lib ${_libpath})
        WIN32_CHECK_LIB("gl" wxmsw${_shortver}${_suffix}_gl.lib wxmsw${_shortver}${_suffix}d_gl.lib ${_libpath})
        WIN32_CHECK_LIB("html" wxmsw${_shortver}${_suffix}_html.lib wxmsw${_shortver}${_suffix}d_html.lib ${_libpath})
        WIN32_CHECK_LIB("media" wxmsw${_shortver}${_suffix}_media.lib wxmsw${_shortver}${_suffix}d_media.lib ${_libpath})
        WIN32_CHECK_LIB("net" wxbase${_shortver}${_suffix}_net.lib wxbase${_shortver}${_suffix}d_net.lib ${_libpath})
        WIN32_CHECK_LIB("odbc" wxbase${_shortver}${_suffix}_odbc.lib wxbase${_shortver}${_suffix}d_odbc.lib ${_libpath})
        WIN32_CHECK_LIB("qa" wxmsw${_shortver}${_suffix}_qa.lib wxmsw${_shortver}${_suffix}d_qa.lib ${_libpath})
        WIN32_CHECK_LIB("richtext" wxmsw${_shortver}${_suffix}_richtext.lib wxmsw${_shortver}${_suffix}d_richtext.lib ${_libpath})
        WIN32_CHECK_LIB("xml" wxbase${_shortver}${_suffix}_xml.lib wxbase${_shortver}${_suffix}d_xml.lib ${_libpath})
        WIN32_CHECK_LIB("xrc" wxmsw${_shortver}${_suffix}_xrc.lib wxmsw${_shortver}${_suffix}d_xrc.lib ${_libpath})
        
        # Contribs
        WIN32_CHECK_LIB("fl" wxmsw${_shortver}${_suffix}_fl.lib wxmsw${_shortver}${_suffix}d_fl.lib ${_libpath})
        WIN32_CHECK_LIB("foldbar" wxmsw${_shortver}${_suffix}_foldbar.lib wxmsw${_shortver}${_suffix}d_foldbar.lib ${_libpath})
        WIN32_CHECK_LIB("gizmos" wxmsw${_shortver}${_suffix}_gizmos.lib wxmsw${_shortver}${_suffix}d_gizmos.lib ${_libpath})
        WIN32_CHECK_LIB("gizmos_xrc" wxmsw${_shortver}${_suffix}_gizmos_xrc.lib wxmsw${_shortver}${_suffix}d_gizmos_xrc.lib ${_libpath})
        WIN32_CHECK_LIB("mmedia" wxmsw${_shortver}${_suffix}_mmedia.lib wxmsw${_shortver}${_suffix}d_mmedia.lib ${_libpath})
        WIN32_CHECK_LIB("netutils" wxmsw${_shortver}${_suffix}_netutils.lib wxmsw${_shortver}${_suffix}d_netutils.lib ${_libpath})
        WIN32_CHECK_LIB("ogl" wxmsw${_shortver}${_suffix}_ogl.lib wxmsw${_shortver}${_suffix}d_ogl.lib ${_libpath})
        WIN32_CHECK_LIB("plot" wxmsw${_shortver}${_suffix}_plot.lib wxmsw${_shortver}${_suffix}d_plot.lib ${_libpath})
        WIN32_CHECK_LIB("stc" wxmsw${_shortver}${_suffix}_stc.lib wxmsw${_shortver}${_suffix}d_stc.lib ${_libpath})
        WIN32_CHECK_LIB("svg" wxmsw${_shortver}${_suffix}_svg.lib wxmsw${_shortver}${_suffix}d_svg.lib ${_libpath})

        # Add some default libraries we'll need
        LIST(APPEND WX_LIBRARIES "debug;wxexpatd.lib" "optimized;wxexpat.lib"
                                 "debug;wxjpegd.lib" "optimized;wxjpeg.lib"
                                 "debug;wxpngd.lib" "optimized;wxpng.lib"
                                 "debug;wxregex${_suffix}d.lib" "optimized;wxregex${_suffix}.lib"
                                 "debug;wxtiffd.lib" "optimized;wxtiff.lib"
                                 "debug;wxzlibd.lib" "optimized;wxzlib.lib")
        LIST(APPEND WX_LIBRARIES winmm comctl32 rpcrt4 wsock32)

        # Preprocessor definitions
        SET(${WX_DEFINITIONS} "-D__WXMSW__")

        IF(NOT _static)
            SET(WX_DEFINITIONS "${WX_DEFINITIONS};-DWXUSINGDLL")
        ENDIF(NOT _static)

        # Bail out if there was an error
        IF(_error)
            SET(WX_FOUND FALSE)
            SET(WX_ROOT_DIR WX_ROOT_DIR-NO_FOUND)
            SET(WX_VERSION_STRING "")
            SET(WX_INCLUDE_DIRS "")
            SET(WX_DEFINITIONS "")
            SET(WX_LIBRARIES "")
            SET(WX_LIBRARY_DIRS "")
            IF(WX_FIND_REQUIRED)
                MESSAGE(FATAL_ERROR "The selected wxWidgets configuration (${_build_desc}) is not available.")
            ELSE(WX_FIND_REQUIRED)
                MESSAGE(STATUS "The selected wxWidgets configuration (${_build_desc}) is not available.")
            ENDIF(WX_FIND_REQUIRED)
        ENDIF(_error)

    ELSE(NOT ${WX_VERSION_STRING} STREQUAL "")
        SET(WX_FOUND FALSE)
        SET(WX_ROOT_DIR WX_ROOT_DIR-NO_FOUND)
        SET(WX_VERSION_STRING "")
        IF(WX_FIND_REQUIRED)
            MESSAGE(FATAL_ERROR "No wxWidgets installation could be found.")
        ELSE(WX_FIND_REQUIRED)
            MESSAGE(STATUS "No wxWidgets installation could be found.")
        ENDIF(WX_FIND_REQUIRED)
    ENDIF(NOT ${WX_VERSION_STRING} STREQUAL "")

# Unix-style
ELSE(WIN32 AND NOT CYGWIN AND NOT MSYS)

    # Set up the wx-config command line
    SET(_args "--version=${_version} --debug=${_debug} --static=${_static} --unicode=${_unicode} ${_modules}")

    IF(NOT $ENV{WXWIN} STREQUAL "")
        FIND_PROGRAM(WX_CONFIG_PATH wx-config
                     PATH $ENV{WXWIN}/bin
                     DOC "Path to the wx-config executable"
                     NO_DEFAULT_PATH)
    ELSE(NOT $ENV{WXWIN} STREQUAL "")
        FIND_PROGRAM(WX_CONFIG_PATH wx-config
                     DOC "Path to the wx-config executable")
    ENDIF(NOT $ENV{WXWIN} STREQUAL "")

    EXEC_PROGRAM(${WX_CONFIG_PATH} ARGS ${_args} --version OUTPUT_VARIABLE WX_VERSION_STRING RETURN_VALUE _retval)

    IF(_retval EQUAL 1)
        SET(WX_FOUND FALSE)
        SET(WX_ROOT_DIR WX_ROOT_DIR-NO_FOUND)
        SET(WX_VERSION_STRING "")
        IF(WX_FIND_REQUIRED)
            MESSAGE(FATAL_ERROR "The selected wxWidgets configuration (${_build_desc}) is not available.")
        ELSE(WX_FIND_REQUIRED)
            MESSAGE(STATUS "The selected wxWidgets configuration (${_build_desc}) is not available.")
        ENDIF(WX_FIND_REQUIRED)
    ENDIF(_retval EQUAL 1)

    IF(NOT _retval)

        SET(WX_FOUND TRUE)

        # Strip the bin and pg_config from the path
        GET_FILENAME_COMPONENT(WX_ROOT_DIR ${WX_CONFIG_PATH} PATH)
        GET_FILENAME_COMPONENT(WX_ROOT_DIR ${WX_ROOT_DIR} PATH)

        EXEC_PROGRAM(${WX_CONFIG_PATH} ARGS ${_args} --cppflags OUTPUT_VARIABLE _cppflags)
        EXEC_PROGRAM(${WX_CONFIG_PATH} ARGS ${_args} --libs OUTPUT_VARIABLE _ldflags)

        # Parse the compiler options
        STRING(STRIP "${_cppflags}" WX_CPPFLAGS)
        SEPARATE_ARGUMENTS(WX_CPPFLAGS)

        # Get the definitions, and drop them from the flags
        STRING(REGEX MATCHALL "-D[^;]+" WX_DEFINITIONS  "${WX_CPPFLAGS}")
        STRING(REGEX REPLACE "-D[^;]+(;|$)" "" WX_CPPFLAGS "${WX_CPPFLAGS}")

        # Get the include dirs. 
        STRING(REGEX MATCHALL "-I[^;]+" WX_INCLUDE_DIRS "${WX_CPPFLAGS}")
        STRING(REPLACE "-I" "" WX_INCLUDE_DIRS "${WX_INCLUDE_DIRS}")
        STRING(REGEX REPLACE "-I[^;]+(;|$)" "" WX_CPPFLAGS "${WX_CPPFLAGS}")

        # Parse the libraries
        STRING(STRIP "${_ldflags}" WX_LIBRARIES)
        SEPARATE_ARGUMENTS(WX_LIBRARIES) 
        STRING(REPLACE "-framework;" "-framework " WX_LIBRARIES "${WX_LIBRARIES}")
        STRING(REPLACE "-arch;" "-arch " WX_LIBRARIES "${WX_LIBRARIES}")
        STRING(REPLACE "-isysroot;" "-isysroot " WX_LIBRARIES "${WX_LIBRARIES}")

        # extract linkdirs (-L) for rpath (i.e., LINK_DIRECTORIES)
        STRING(REGEX MATCHALL "-L[^;]+" WX_LIBRARY_DIRS "${WX_LIBRARIES}")
        STRING(REPLACE "-L" "" WX_LIBRARY_DIRS "${WX_LIBRARY_DIRS}")

    ELSE(NOT _retval)
        SET(WX_FOUND FALSE)
        SET(WX_ROOT_DIR WX_ROOT_DIR-NO_FOUND)
        SET(WX_VERSION_STRING "")
        IF(WX_FIND_REQUIRED)
            MESSAGE(FATAL_ERROR "No wxWidgets installation could be found.")
        ELSE(WX_FIND_REQUIRED)
            MESSAGE(STATUS "No wxWidgets installation could be found.")
        ENDIF(WX_FIND_REQUIRED)
    ENDIF(NOT _retval)

ENDIF(WIN32 AND NOT CYGWIN AND NOT MSYS)
