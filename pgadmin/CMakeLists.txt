#######################################################################
#
# pgAdmin3 - PostgreSQL tools
# Copyright (C) 2002 - 2008, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# $Id$
#
# CMakeLists.txt - CMake build configuration
#
#######################################################################

################################################################################
# Set up the build for the pgadmin binary
################################################################################
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

FILE(GLOB_RECURSE _cpp_files *.cpp)
FILE(GLOB_RECURSE _cc_files *.cc)
FILE(GLOB_RECURSE _h_files include/*.h)

# C files need special treatment
SET(pgadmin_extra_src 
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/tabcomplete.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/db/keywords.c)


################################################################################
# Enable building of i18n files
################################################################################
SET(ENABLE_NLS YES CACHE BOOL "Enable building NLS functionality")
IF(ENABLE_NLS)
  FIND_PROGRAM(MSGFMT_EXECUTABLE msgfmt)

  # Add a custom output for each po file we need to compile
  IF(MSGFMT_EXECUTABLE)

    FILE(GLOB_RECURSE _pofiles ${pgadmin3_SOURCE_DIR}/i18n/??_??/pgadmin3.po)

    FOREACH(_pofile ${_pofiles})
        GET_FILENAME_COMPONENT(_dir ${_pofile} PATH)
        GET_FILENAME_COMPONENT(_lang ${_dir} NAME)

        SET(_out "${pgadmin3_BINARY_DIR}/pgadmin/CMakeFiles/pgadmin.dir/pgadmin3.${_lang}.mo")

        IF(APPLE)
            # Install the translations into the app bundle
            ADD_CUSTOM_COMMAND(
                OUTPUT ${_out}
                COMMAND ${MSGFMT_EXECUTABLE} -o ${_out} ${_pofile}
                COMMAND mkdir -p ${pgadmin3_BINARY_DIR}/pgadmin/pgadmin.app/Contents/Resources/i18n/${_lang}
                COMMAND cp ${_out} ${pgadmin3_BINARY_DIR}/pgadmin/pgadmin.app/Contents/Resources/i18n/${_lang}/pgadmin3.mo
                COMMAND cp ${_dir}/wxstd.mo ${pgadmin3_BINARY_DIR}/pgadmin/pgadmin.app/Contents/Resources/i18n/${_lang}/wxstd.mo 
                DEPENDS ${_pofile}
            )
        ELSE(APPLE)
            # Install everything the 'normal' way
            ADD_CUSTOM_COMMAND(
                OUTPUT ${_out}
                COMMAND ${MSGFMT_EXECUTABLE} -o ${_out} ${_pofile}
                DEPENDS ${_pofile}
            )
            INSTALL(
                FILES ${_out}
                DESTINATION share/pgadmin3/i18n/${_lang}/
                RENAME pgadmin3.mo
            )   
            IF(EXISTS "${_dir}/wxstd.mo")
                INSTALL(
                    FILES "${_dir}/wxstd.mo"
                    DESTINATION share/pgadmin3/i18n/${_lang}/
                )
            ENDIF(EXISTS "${_dir}/wxstd.mo")
        ENDIF(APPLE)

        SET(_translations ${_translations} ${_out})
    ENDFOREACH(_pofile)
    INSTALL(FILES ${pgadmin3_SOURCE_DIR}/i18n/pgadmin3.lng DESTINATION share/pgadmin3/i18n)
  ELSE(MSGFMT_EXECUTABLE)
    MESSAGE(SEND_ERROR "Could not find msgfmt - you need to build with ENABLE_NLS=NO")
  ENDIF(MSGFMT_EXECUTABLE)
ENDIF(ENABLE_NLS)


################################################################################
# Now setup the actual target
################################################################################

SET(pgadmin_srcs ${_cpp_files} ${_cc_files} ${_h_files} ${pgadmin_extra_src} ${_translations})
 
# On windows, add the resource file with version number and icon reference
IF(WIN32)
  SET(pgadmin_srcs ${pgadmin_srcs} pgAdmin3.rc)
ENDIF(WIN32)

# Create the target
ADD_EXECUTABLE(pgadmin WIN32 MACOSX_BUNDLE ${pgadmin_srcs})
TARGET_LINK_LIBRARIES(pgadmin ${PG_LIBRARIES} ${WX_LIBRARIES} ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES})

# Add subversion tag rule and "fake target"
IF(WIN32)
  ADD_CUSTOM_TARGET(svnversion.h ALL
                    COMMAND ${pgadmin3_SOURCE_DIR}/pgadmin/ver_svn.bat
                    WORKING_DIRECTORY ${pgadmin3_SOURCE_DIR}/pgadmin)
ELSE(WIN32)
  ADD_CUSTOM_TARGET(svnversion.h ALL
                    COMMAND ${pgadmin3_SOURCE_DIR}/pgadmin/ver_svn.sh
                    WORKING_DIRECTORY ${pgadmin3_SOURCE_DIR}/pgadmin)
ENDIF(WIN32)
ADD_DEPENDENCIES(pgadmin svnversion.h)
SET_SOURCE_FILES_PROPERTIES(${pgadmin3_SOURCE_DIR}/pgadmin/frm/frmAbout.cpp PROPERTIES
                            OBJECT_DEPENDS ${pgadmin3_SOURCE_DIR}/pgadmin/include/svnversion.h)

# Add extra compile flags, if used on this platform
IF(EXTRA_COMPILE_FLAGS)
  SET_TARGET_PROPERTIES(pgadmin PROPERTIES COMPILE_FLAGS ${EXTRA_COMPILE_FLAGS})
ENDIF(EXTRA_COMPILE_FLAGS)

# Make sure we install the file
INSTALL(TARGETS pgadmin DESTINATION bin)

