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

# Mac template platform definition CMake file
# Included from ../CMakeLists.txt
include(${CMAKE_CURRENT_SOURCE_DIR}/Common/common.cmake)

# remember that the current source dir is the project root; this file is in Mac/
file (GLOB PLATFORM RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Mac/[^.]*.cpp
    Mac/[^.]*.h
    Mac/[^.]*.cmake
    )

# GCC options
add_definitions(-std=c++0x)

include_directories(Rootfs/include/linphone)
include_directories(Rootfs/include)

SOURCE_GROUP(Mac FILES ${PLATFORM})

set (SOURCES
    ${SOURCES}
    ${PLATFORM}
    )

set(PLIST "Mac/bundle_template/Info.plist")
set(STRINGS "Mac/bundle_template/InfoPlist.strings")
set(LOCALIZED "Mac/bundle_template/Localized.r")

add_mac_plugin(${PROJECT_NAME} ${PLIST} ${STRINGS} ${LOCALIZED} SOURCES)

# add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!
target_link_libraries(${PROJECT_NAME}
        ${PLUGIN_INTERNAL_DEPS}
        "${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/liblinphone.5.dylib"
        "${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libmediastreamer.1.dylib"
        "${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libortp.8.dylib"
)

# fix output path
set_target_properties (${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${FB_BIN_DIR}/${PLUGIN_NAME}"
        LIBRARY_OUTPUT_DIRECTORY "${FB_BIN_DIR}/${PLUGIN_NAME}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${FB_BIN_DIR}/${PLUGIN_NAME}/Debug"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${FB_BIN_DIR}/${PLUGIN_NAME}/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${FB_BIN_DIR}/${PLUGIN_NAME}/Release"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${FB_BIN_DIR}/${PLUGIN_NAME}/Release"
)

get_target_property(PLUGIN_EXT ${PROJECT_NAME} BUNDLE_EXTENSION)
SET (FB_PACKAGE_SUFFIX Mac)
SET (LINPHONEWEB_SHAREDIR linphoneweb)
SET (FB_OUT_DIR ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR})
SET (FB_BUNDLE_DIR ${FB_OUT_DIR}/${PLUGIN_NAME}.${PLUGIN_EXT}/Contents/MacOS)

# Copy dll dependencies
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} 
                 POST_BUILD
                 COMMAND ${CMAKE_COMMAND} -E remove_directory ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libavcodec.54.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libavutil.51.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libcrypto.1.0.0.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libeXosip2.7.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/liblinphone.5.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libmediastreamer.1.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libogg.0.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libortp.8.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libosip2.7.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libosipparser2.7.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libspeex.1.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libspeexdsp.1.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libssl.1.0.0.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libswscale.2.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libtheora.0.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libz.1.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/

# FFMPEG stuff
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libSDL-1.2.0.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libvorbisenc.2.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libvorbis.0.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libtheoraenc.1.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libtheoradec.1.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libschroedinger-1.0.0.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libopenjpeg.1.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libmp3lame.0.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libdirac_encoder.0.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libdirac_decoder.0.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libbz2.1.0.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/liborc-0.4.0.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libmodplug.1.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/

                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/share/
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share/sounds/linphone/ringback.wav ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share/sounds/linphone/rings/oldphone.wav ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/

# Change rpath
                 COMMAND COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/mac_rpath.py ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
                 COMMAND COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/mac_rpath.py ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/ ${FB_BUNDLE_DIR}/${PLUGIN_NAME}
)


###############################################################################
# XPI Package
function (create_xpi_package PROJNAME PROJVERSION OUTDIR)
    set (WIX_SOURCES
            ${FB_ROOT}/cmake/dummy.cpp
        )
	if (NOT FB_XPI_PACKAGE_SUFFIX)
		set (FB_XPI_PACKAGE_SUFFIX _PKG_XPI)
	endif()
	
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Mac/XPI/install.rdf ${CMAKE_CURRENT_BINARY_DIR}/install.rdf)
	
	SET(FB_PKG_DIR ${FB_OUT_DIR}/XPI)
	get_target_property(ONAME ${PROJNAME} OUTPUT_NAME)
	
	ADD_LIBRARY(${PROJNAME}${FB_XPI_PACKAGE_SUFFIX} STATIC ${WIX_SOURCES})
	ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}${FB_XPI_PACKAGE_SUFFIX}
                 POST_BUILD
                 COMMAND ${CMAKE_COMMAND} -E remove_directory ${FB_PKG_DIR}
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/install.rdf ${FB_PKG_DIR}/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Mac/XPI/chrome.manifest ${FB_PKG_DIR}/
                 
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/chrome/skin
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Common/icon48.png ${FB_PKG_DIR}/chrome/skin/
                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Common/icon64.png ${FB_PKG_DIR}/chrome/skin/
                 
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/plugins/
                 COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/copy.py ${FB_OUT_DIR}/${PLUGIN_NAME}.${PLUGIN_EXT} ${FB_PKG_DIR}/plugins/${PLUGIN_NAME}.${PLUGIN_EXT}
                 COMMAND jar cfM ${FB_OUT_DIR}/${PROJNAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.xpi -C ${FB_PKG_DIR} .
	)
	ADD_DEPENDENCIES(${PROJNAME}${FB_XPI_PACKAGE_SUFFIX} ${PROJNAME})
	message("-- Successfully added XPI package step")
endfunction(create_xpi_package)
###############################################################################


###############################################################################
# PKG Package
function (create_pkg_package PROJNAME PROJVERSION OUTDIR)
    set (WIX_SOURCES
            ${FB_ROOT}/cmake/dummy.cpp
        )
	if (NOT FB_PKG_PACKAGE_SUFFIX)
		set (FB_PKG_PACKAGE_SUFFIX _PKG_PKG)
	endif()
	
	file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/PKG.pmdoc)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Mac/PKG.pmdoc/move.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/PKG.pmdoc/)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Mac/PKG.pmdoc/index.xml ${CMAKE_CURRENT_BINARY_DIR}/PKG.pmdoc/index.xml)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Mac/PKG.pmdoc/01linphone.xml ${CMAKE_CURRENT_BINARY_DIR}/PKG.pmdoc/01linphone.xml)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Mac/PKG.pmdoc/01linphone-contents.xml ${CMAKE_CURRENT_BINARY_DIR}/PKG.pmdoc/01linphone-contents.xml)
	
	ADD_LIBRARY(${PROJNAME}${FB_PKG_PACKAGE_SUFFIX} STATIC ${WIX_SOURCES})
	ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}${FB_PKG_PACKAGE_SUFFIX}
                 POST_BUILD
                 COMMAND ${CMAKE_COMMAND} -E remove_directory ${OUTDIR}/PKG.pmdoc
                 COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/copy.py ${CMAKE_CURRENT_BINARY_DIR}/PKG.pmdoc ${OUTDIR}/PKG.pmdoc
                 COMMAND packagemaker --doc ${OUTDIR}/PKG.pmdoc --out ${OUTDIR}/${PROJNAME}.pkg
	)
	ADD_DEPENDENCIES(${PROJNAME}${FB_PKG_PACKAGE_SUFFIX} ${PROJNAME})
	message("-- Successfully added PKG package step")
endfunction(create_pkg_package)
###############################################################################

create_pkg_package(${PLUGIN_NAME} ${FBSTRING_PLUGIN_VERSION} ${FB_OUT_DIR})
create_xpi_package(${PLUGIN_NAME} ${FBSTRING_PLUGIN_VERSION} ${FB_OUT_DIR})

create_signed_xpi(${PLUGIN_NAME} 
	"${FB_OUT_DIR}/XPI/"
	"${FB_OUT_DIR}/${PROJECT_NAME}-${FBSTRING_PLUGIN_VERSION}-${FB_PACKAGE_SUFFIX}.xpi"
	"${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pem"
	"${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
	${PLUGIN_NAME}_PKG_XPI
)
