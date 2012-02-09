#/**********************************************************\ 
# 
# Auto-generated CMakeLists.txt for the linphone project
#
#\**********************************************************/

# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

Project(${PLUGIN_NAME}-js)

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    [^.]*.cpp
    [^.]*.h
    [^.]*.cmake
    )

include_directories(${PLUGIN_INCLUDE_DIRS})

#pkg-config
find_package(PkgConfig)

#include liblinphone
pkg_check_modules(LINPHONE REQUIRED linphone>=2.5)
link_directories(${LINPHONE_STATIC_LINK_DIRS})
include_directories(${LINPHONE_STATIC_INCLUDE_DIRS})

# Generated files are stored in ${GENERATED} by the project configuration
SET_SOURCE_FILES_PROPERTIES(
    ${GENERATED}
    PROPERTIES
        GENERATED 1
    )

SOURCE_GROUP(Generated FILES
    ${GENERATED}
    )

SET( SOURCES
    ${GENERAL}
    ${GENERATED}
    )

# This will include Win/projectDef.cmake, X11/projectDef.cmake, Mac/projectDef 
# depending on the platform
include_platform()


#link liblinphone
target_link_libraries(${PROJECT_NAME} ${LINPHONE_STATIC_LIBRARIES})