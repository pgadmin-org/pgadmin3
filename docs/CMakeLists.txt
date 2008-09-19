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
# Build rules for installing the documentation
################################################################################

# XXX: should be completed with autorules to build the HHC files
FILE(GLOB_RECURSE _tipsfiles ${CMAKE_CURRENT_SOURCE_DIR}/??_??/tips.txt)
FOREACH (_tipsfile ${_tipsfiles})
    GET_FILENAME_COMPONENT(_dir ${_tipsfile} PATH)
    INSTALL(
        DIRECTORY ${_dir} 
        DESTINATION share/pgadmin3/docs 
        PATTERN .svn EXCLUDE
    )
ENDFOREACH (_tipsfile)

