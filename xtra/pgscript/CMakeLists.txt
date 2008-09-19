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
# Set up the build for the pgscript standalone binary
################################################################################
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../pgadmin/include)

FILE(GLOB_RECURSE _cpp_files ../../pgadmin/pgscript/*.cpp ../../pgadmin/pgscript/*.cc)
FILE(GLOB_RECURSE _h_files ../../pgadmin/include/*.h)

# Add sourcefiles not in the pgscript directory
SET(pgscript_srcs ${_cpp_files} ${_h_files} 
   ${CMAKE_CURRENT_SOURCE_DIR}/../../pgadmin/utils/misc.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/../../pgadmin/utils/md5.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/../../pgadmin/utils/sysLogger.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/../../pgadmin/db/pgSet.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/../../pgadmin/db/pgQueryThread.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/../../pgadmin/db/pgConn.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/bin/pgsMain.cpp
)

# Defines to specify that we're building the console mode pgscript binary
ADD_DEFINITIONS(-D PGSCLI -DPGADMIN3_H)

ADD_EXECUTABLE(pgscript ${pgscript_srcs})
TARGET_LINK_LIBRARIES(pgscript ${PG_LIBRARIES} ${WX_LIBRARIES})

INSTALL(TARGETS pgscript DESTINATION bin)

