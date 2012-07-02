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
include(${CMAKE_CURRENT_SOURCE_DIR}/Common/common.cmake)

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

SET (LINPHONEWEB_SHAREDIR linphoneweb)
SET (FB_OUT_DIR ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR})

# Copy dll dependencies
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} 
		  PRE_BUILD
		  COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libavcodec.so.53 ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libavutil.so.51 ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libcrypto.so.1.0.0 ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libeXosip2.so.7 ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libjpeg.so.8 ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/liblinphone.so.5 ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libmediastreamer.so.1 ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libogg.so.0 ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libortp.so.8 ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libosip2.so.7 ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libosipparser2.so.7 ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libspeex.so.1 ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libspeexdsp.so.1 ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libssl.so.1.0.0 ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/
          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libswscale.so.2 ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libtheora.so.0 ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libv4l1.so.0 ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libv4l2.so.0 ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libv4lconvert.so.0 ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libvpx.so.1 ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libz.so.1 ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/

		  COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/share/
		  COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/
		  COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/
		  COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share/sounds/linphone/ringback.wav ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share/sounds/linphone/rings/oldphone.wav ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/
)

# Use default chrpath if not defined
IF(NOT DEFINED CMAKE_CHRPATH)
	SET(CMAKE_CHRPATH chrpath)
ENDIF(NOT DEFINED CMAKE_CHRPATH)

# Change rpath
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} 
                 PRE_BUILD
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libavcodec.so.53
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libavutil.so.51
#                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libcrypto.so.1.0.0                 
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libjpeg.so.8
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libeXosip2.so.7
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/liblinphone.so.5
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libmediastreamer.so.1           
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libogg.so.0
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libortp.so.8
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libosip2.so.7
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libosipparser2.so.7
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libspeex.so.1
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libspeexdsp.so.1
#                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libssl.so.1.0.0
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libswscale.so.2
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libtheora.so.0
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libv4l1.so.0
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libv4l2.so.0
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libv4lconvert.so.0
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libvpx.so.1
                 COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libz.so.1
)

# Add rpath to generated library
SET (CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,'\$ORIGIN/${LINPHONEWEB_SHAREDIR}'" )

###############################################################################
# TGZ Package
function (create_tgz_package PROJNAME PROJVERSION OUTDIR)
    set (WIX_SOURCES
            ${FB_ROOT}/cmake/dummy.cpp
        )
	if (NOT FB_TGZ_PACKAGE_SUFFIX)
		set (FB_TGZ_PACKAGE_SUFFIX _PKG_TGZ)
	endif()
	
	set(FB_PKG_DIR ${FB_OUT_DIR}/TGZ/)
	get_target_property(ONAME ${PROJNAME} OUTPUT_NAME)
	
	set(PKG_PREFIX ${PROJECT_NAME}-${FBSTRING_PLUGIN_VERSION})
	
	ADD_LIBRARY(${PROJNAME}${FB_TGZ_PACKAGE_SUFFIX} STATIC ${WIX_SOURCES})
	ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}${FB_TGZ_PACKAGE_SUFFIX}
                 POST_BUILD
                 COMMAND ${CMAKE_COMMAND} -E remove_directory ${FB_PKG_DIR}
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/${PKG_PREFIX}/
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${ONAME}.so ${FB_PKG_DIR}/${PKG_PREFIX}/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libavcodec.so.53 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libavcodec.so.53 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libavutil.so.51 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libcrypto.so.1.0.0 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libeXosip2.so.7 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libjpeg.so.8 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/liblinphone.so.5 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libmediastreamer.so.1 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libogg.so.0 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libortp.so.8 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libosip2.so.7 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libosipparser2.so.7 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libspeex.so.1 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libspeexdsp.so.1 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libssl.so.1.0.0 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libswscale.so.2 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libtheora.so.0 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libv4l1.so.0 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libv4l2.so.0 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libv4lconvert.so.0 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libvpx.so.1 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libz.so.1 ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/

                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/share/
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/share/sounds/
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/ringback.wav ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/oldphone.wav ${FB_PKG_DIR}/${PKG_PREFIX}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/
                 COMMAND tar zcvf "${OUTDIR}/${PROJECT_NAME}-${FBSTRING_PLUGIN_VERSION}-Linux.tar.gz" 
                 		-C ${FB_PKG_DIR} ${PKG_PREFIX}
	)
	ADD_DEPENDENCIES(${PROJNAME}${FB_TGZ_PACKAGE_SUFFIX} ${PROJNAME})
	message("-- Successfully added TGZ package step")
endfunction(create_tgz_package)
###############################################################################

###############################################################################
# XPI Package
function (create_xpi_package PROJNAME PROJVERSION OUTDIR)
    set (WIX_SOURCES
            ${FB_ROOT}/cmake/dummy.cpp
        )
	if (NOT FB_XPI_PACKAGE_SUFFIX)
		set (FB_XPI_PACKAGE_SUFFIX _PKG_XPI)
	endif()
	
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/X11/XPI/install.rdf ${CMAKE_CURRENT_BINARY_DIR}/install.rdf)
	
	set(FB_PKG_DIR ${FB_OUT_DIR}/XPI/)
	get_target_property(ONAME ${PROJNAME} OUTPUT_NAME)
	
	ADD_LIBRARY(${PROJNAME}${FB_XPI_PACKAGE_SUFFIX} STATIC ${WIX_SOURCES})
	ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}${FB_XPI_PACKAGE_SUFFIX}
                 POST_BUILD
                 COMMAND ${CMAKE_COMMAND} -E remove_directory ${FB_PKG_DIR}
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/install.rdf ${FB_PKG_DIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/X11/XPI/chrome.manifest ${FB_PKG_DIR}/
                 
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/chrome/skin
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Common/icon48.png ${FB_PKG_DIR}/chrome/skin/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Common/icon64.png ${FB_PKG_DIR}/chrome/skin/
                 
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/plugins/
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${ONAME}.so ${FB_PKG_DIR}/plugins/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libavcodec.so.53 ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libavutil.so.51 ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libcrypto.so.1.0.0 ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libeXosip2.so.7 ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libjpeg.so.8 ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/liblinphone.so.5 ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libmediastreamer.so.1 ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libogg.so.0 ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libortp.so.8 ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libosip2.so.7 ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libosipparser2.so.7 ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libspeex.so.1 ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libspeexdsp.so.1 ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libssl.so.1.0.0 ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libswscale.so.2 ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libtheora.so.0 ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libv4l1.so.0 ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libv4l2.so.0 ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libv4lconvert.so.0 ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libvpx.so.1 ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libz.so.1 ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/

                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/share/
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/share/sounds/
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/ringback.wav ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/oldphone.wav ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/
                 COMMAND "${Java_JAR_EXECUTABLE}" "cfM" "${OUTDIR}/${PROJECT_NAME}-${PROJVERSION}-Linux-unsigned.xpi" 
                 		-C ${FB_PKG_DIR} .
	)
	ADD_DEPENDENCIES(${PROJNAME}${FB_XPI_PACKAGE_SUFFIX} ${PROJNAME})
	message("-- Successfully added XPI package step")
endfunction(create_xpi_package)
###############################################################################


###############################################################################
# CRX Package
function (create_crx_package PROJNAME PROJVERSION OUTDIR)
    set (WIX_SOURCES
            ${FB_ROOT}/cmake/dummy.cpp
        )
	if (NOT FB_CRX_PACKAGE_SUFFIX)
		set (FB_CRX_PACKAGE_SUFFIX _PKG_CRX)
	endif()
	
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/X11/CRX/manifest.json ${CMAKE_CURRENT_BINARY_DIR}/manifest.json)
	
	set(FB_PKG_DIR ${FB_OUT_DIR}/CRX/)
	get_target_property(ONAME ${PROJNAME} OUTPUT_NAME)
	
	ADD_LIBRARY(${PROJNAME}${FB_CRX_PACKAGE_SUFFIX} STATIC ${WIX_SOURCES})
	ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}${FB_CRX_PACKAGE_SUFFIX}
                 POST_BUILD
                 COMMAND ${CMAKE_COMMAND} -E remove_directory ${FB_PKG_DIR}
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${ONAME}.so ${FB_PKG_DIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/manifest.json ${FB_PKG_DIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Common/icon48.png ${FB_PKG_DIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Common/icon64.png ${FB_PKG_DIR}/
                 
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libavcodec.so.53 ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libavutil.so.51 ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libcrypto.so.1.0.0 ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libeXosip2.so.7 ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libjpeg.so.8 ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/liblinphone.so.5 ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libmediastreamer.so.1 ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libogg.so.0 ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libortp.so.8 ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libosip2.so.7 ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libosipparser2.so.7 ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libspeex.so.1 ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libspeexdsp.so.1 ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libssl.so.1.0.0 ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libswscale.so.2 ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libtheora.so.0 ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libv4l1.so.0 ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libv4l2.so.0 ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libv4lconvert.so.0 ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libvpx.so.1 ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/libz.so.1 ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/

                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/share/
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/ringback.wav ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/oldphone.wav ${FB_PKG_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/
                 COMMAND "${Java_JAR_EXECUTABLE}" "cfM" "${OUTDIR}/${PROJECT_NAME}-${PROJVERSION}-Linux-unsigned.crx" 
                 		-C ${FB_PKG_DIR} .
	)
	ADD_DEPENDENCIES(${PROJNAME}${FB_CRX_PACKAGE_SUFFIX} ${PROJNAME})
	message("-- Successfully added CRX package step")
endfunction(create_crx_package)
###############################################################################

create_tgz_package(${PLUGIN_NAME} ${FBSTRING_PLUGIN_VERSION} ${FB_OUT_DIR})
create_xpi_package(${PLUGIN_NAME} ${FBSTRING_PLUGIN_VERSION} ${FB_OUT_DIR})
create_crx_package(${PLUGIN_NAME} ${FBSTRING_PLUGIN_VERSION} ${FB_OUT_DIR})

create_signed_xpi(${PLUGIN_NAME} 
	"${FB_OUT_DIR}/${PROJECT_NAME}-${FBSTRING_PLUGIN_VERSION}-Linux-unsigned.xpi"
	"${FB_OUT_DIR}/${PROJECT_NAME}-${FBSTRING_PLUGIN_VERSION}-Linux.xpi"
	"${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pem"
	"${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
	${PLUGIN_NAME}_PKG_XPI
)
