##
# Linphone Web - Web plugin of Linphone an audio/video SIP phone
# Copyright (C) 2012-2014 Belledonne Communications
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
##

# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

# Force use of python 2.7 as m2crypto is not available for python 3
set(Python_ADDITIONAL_VERSIONS 2.7)

Project(${PLUGIN_NAME})

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Src/[^.]*.cpp
    Src/[^.]*.h
    [^.]*.cmake
    )

file (GLOB TRANSFERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Transfers/[^.]*.cpp
    Transfers/[^.]*.h
    Transfers/[^.]*.cmake
    )

include_directories(${PLUGIN_INCLUDE_DIRS})
include_directories(${FB_UTF8_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Generated files are stored in ${GENERATED} by the project configuration
SET_SOURCE_FILES_PROPERTIES(
    ${GENERATED}
    PROPERTIES
        GENERATED 1
    )

SOURCE_GROUP(Generated FILES ${GENERATED})
SOURCE_GROUP(Transfers FILES ${TRANSFERS})

SET(SOURCES
    ${GENERAL}
    ${TRANSFERS}
    ${GENERATED}
    )

add_definitions("-DBOOST_FILESYSTEM_VERSION=3")
add_boost_library(filesystem)

if(CMAKE_BUILD_TYPE MATCHES Debug)
	add_firebreath_library(log4cplus)
	add_definitions("-DDEBUG")
endif()

# Maximum warnings
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	add_definitions("-Werror")
endif()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	add_definitions("-Werror")
	add_definitions("-Wno-unused-private-field")
endif()

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/Stage CACHE PATH "Install prefix" FORCE)
set(CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX} CACHE PATH "Prefix path" FORCE)

# Include linphone-cmake-builder
set(LINPHONE_BUILDER_CONFIG_FILE "configs/config-webplugin.cmake" CACHE STRING "Path to the linphone builder configuration file to build linphone core library." FORCE)
set(LINPHONE_BUILDER_WORK_DIR "${CMAKE_BINARY_DIR}/Core" CACHE PATH "Working directory to build linphone core library." FORCE)
add_subdirectory(Core)

# This will include Win/projectDef.cmake, X11/projectDef.cmake, Mac/projectDef 
# depending on the platform
include_platform()
