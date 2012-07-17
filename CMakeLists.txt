##
# Linphone Web - Web plugin of Linphone an audio/video SIP phone
# Copyright (C) 2012  Yann Diorcet <yann.diorcet@linphone.org>
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

Project(${PLUGIN_NAME})

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    [^.]*.cpp
    [^.]*.h
    [^.]*.cmake
    )

include_directories(${PLUGIN_INCLUDE_DIRS})

# Generated files are stored in ${GENERATED} by the project configuration
SET_SOURCE_FILES_PROPERTIES(
    ${GENERATED}
    PROPERTIES
        GENERATED 1
    )

SOURCE_GROUP(Generated FILES
    ${GENERATED}
    )

SET(SOURCES
    ${GENERAL}
    ${GENERATED}
    )

# This will include Win/projectDef.cmake, X11/projectDef.cmake, Mac/projectDef 
# depending on the platform
include_platform()