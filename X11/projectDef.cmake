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

# X11 template platform definition CMake file
# Included from ../CMakeLists.txt

# remember that the current source dir is the project root; this file is in X11/
file (GLOB PLATFORM RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    X11/[^.]*.cpp
    X11/[^.]*.h
    X11/[^.]*.cmake
    )

# GCC options
add_definitions(-std=c++0x)

include_directories(Rootfs/include/linphone)
include_directories(Rootfs/include)

SOURCE_GROUP(X11 FILES ${PLATFORM})

set (SOURCES
    ${SOURCES}
    ${PLATFORM}
    )

add_x11_plugin(${PROJECT_NAME} SOURCES)

# Add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!
target_link_libraries(${PROJECT_NAME} 
	${PLUGIN_INTERNAL_DEPS}
	"${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/liblinphone.so.5"
	"${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libmediastreamer.so.1"
	"${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libortp.so.8"
)

# Copy dll dependencies
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} 
		  PRE_BUILD
		  COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libavcodec.so.53 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libavutil.so.51 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libeXosip2.so.7 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libjpeg.so.8 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/liblinphone.so.5 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libmediastreamer.so.1 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libogg.so.0 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libortp.so.8 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libosip2.so.7 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libosipparser2.so.7 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libspeex.so.1 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libspeexdsp.so.1 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libswscale.so.2 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libtheora.so.0 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libv4l1.so.0 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libv4l2.so.0 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libv4lconvert.so.0 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libz.so.1 ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/

		  COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/share/
		  COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/share/sounds/
		  COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/share/sounds/linphone/
		  COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/share/sounds/linphone/rings/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share/sounds/linphone/ringback.wav ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/share/sounds/linphone/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share/sounds/linphone/rings/oldphone.wav ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/share/sounds/linphone/rings/
)

# Use default chrpath if not defined
IF(NOT DEFINED CMAKE_CHRPATH)
	SET(CMAKE_CHRPATH chrpath)
ENDIF(NOT DEFINED CMAKE_CHRPATH)

# Change rpath
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} 
                 PRE_BUILD
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libavcodec.so.53
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libavutil.so.51
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libjpeg.so.8
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libeXosip2.so.7
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/liblinphone.so.5
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libmediastreamer.so.1           
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libogg.so.0
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libortp.so.8
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libosip2.so.7
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libosipparser2.so.7
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libspeex.so.1
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libspeexdsp.so.1
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libswscale.so.2
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libtheora.so.0
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libv4l1.so.0
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libv4l2.so.0
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libv4lconvert.so.0
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libz.so.1
)

# Add rpath to generated library
SET (CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,'\$ORIGIN/linphoneweb_libs'" )

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/X11/XPI/install.rdf DESTINATION .)
install(TARGETS ${PROJECT_NAME} DESTINATION plugins)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libavcodec.so.53 DESTINATION plugins/linphoneweb_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libavutil.so.51 DESTINATION plugins/linphoneweb_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libjpeg.so.8 DESTINATION plugins/linphoneweb_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libeXosip2.so.7 DESTINATION plugins/linphoneweb_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/liblinphone.so.5 DESTINATION plugins/linphoneweb_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libmediastreamer.so.1 DESTINATION plugins/linphoneweb_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libogg.so.0 DESTINATION plugins/linphoneweb_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libortp.so.8 DESTINATION plugins/linphoneweb_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libosip2.so.7 DESTINATION plugins/linphoneweb_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libosipparser2.so.7 DESTINATION plugins/linphoneweb_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libspeex.so.1 DESTINATION plugins/linphoneweb_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libspeexdsp.so.1 DESTINATION plugins/linphoneweb_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libswscale.so.2 DESTINATION plugins/linphoneweb_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libtheora.so.0 DESTINATION plugins/linphoneweb_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libv4l1.so.0 DESTINATION plugins/linphoneweb_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libv4l2.so.0 DESTINATION plugins/linphoneweb_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libv4lconvert.so.0 DESTINATION plugins/linphoneweb_libs)
install(FILES ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/linphoneweb_libs/libz.so.1 DESTINATION plugins/linphoneweb_libs)

SET(CPACK_PACKAGE_NAME ${PROJECT_NAME})
SET(CPACK_PACKAGE_VERSION ${FBSTRING_PLUGIN_VERSION})
SET(CPACK_GENERATOR "ZIP")
SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY "0")
INCLUDE(CPack)
