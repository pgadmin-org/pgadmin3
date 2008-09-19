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
# Build rules for installing the branding files
################################################################################

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/branding.ini DESTINATION share/pgadmin3/branding)

