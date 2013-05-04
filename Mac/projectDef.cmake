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
INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/Common/common.cmake)

# remember that the current source dir is the project root; this file is in Mac/
FILE(GLOB PLATFORM RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	Mac/[^.]*.cpp
	Mac/[^.]*.mm
	Mac/[^.]*.h
	Mac/[^.]*.cmake
)

# GCC options
ADD_DEFINITIONS(
	-DCORE_THREADED
)

INCLUDE_DIRECTORIES(Rootfs/include/linphone)
INCLUDE_DIRECTORIES(Rootfs/include)

SOURCE_GROUP(Mac FILES ${PLATFORM})

SET(SOURCES
	${SOURCES}
	${PLATFORM})

SET(PLIST "Mac/bundle_template/Info.plist")
SET(STRINGS "Mac/bundle_template/InfoPlist.strings")
SET(LOCALIZED "Mac/bundle_template/Localized.r")

add_mac_plugin(${PROJECT_NAME} ${PLIST} ${STRINGS} ${LOCALIZED} SOURCES)
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES FOLDER ${FBSTRING_ProductName})

FIND_LIBRARY(OPENGL_FRAMEWORK OpenGL)
FIND_LIBRARY(QUARTZ_CORE_FRAMEWORK QuartzCore)

# add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		${PLUGIN_INTERNAL_DEPS}
		"${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/liblinphone.5.dylib"
		"${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libmediastreamer_base.3.dylib"
		"${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libmediastreamer_voip.3.dylib"
		"${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libortp.9.dylib"
		${OPENGL_FRAMEWORK}
		${QUARTZ_CORE_FRAMEWORK}
)

# fix output path
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${FB_BIN_DIR}/${PLUGIN_NAME}"
		LIBRARY_OUTPUT_DIRECTORY "${FB_BIN_DIR}/${PLUGIN_NAME}"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG "${FB_BIN_DIR}/${PLUGIN_NAME}/Debug"
		LIBRARY_OUTPUT_DIRECTORY_DEBUG "${FB_BIN_DIR}/${PLUGIN_NAME}/Debug"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE "${FB_BIN_DIR}/${PLUGIN_NAME}/Release"
		LIBRARY_OUTPUT_DIRECTORY_RELEASE "${FB_BIN_DIR}/${PLUGIN_NAME}/Release"
)

GET_TARGET_PROPERTY(PLUGIN_EXT ${PROJECT_NAME} BUNDLE_EXTENSION)
SET(FB_PACKAGE_SUFFIX Mac)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	SET(FB_PACKAGE_SUFFIX "${FB_PACKAGE_SUFFIX}-x86_64")
else(CMAKE_SIZEOF_VOID_P EQUAL 8 )
	SET(FB_PACKAGE_SUFFIX "${FB_PACKAGE_SUFFIX}-x86")
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
SET(LINPHONEWEB_SHAREDIR linphoneweb)
SET(FB_OUT_DIR ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR})
SET(FB_BUNDLE_DIR ${FB_OUT_DIR}/${FBSTRING_PluginFileName}.${PLUGIN_EXT}/Contents/MacOS)

# Copy dll dependencies
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} 
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libavcodec.53.61.100.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libavutil.51.35.100.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libcrypto.1.0.0.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libeXosip2.7.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/liblinphone.5.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libmediastreamer_base.3.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libmediastreamer_voip.3.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libogg.0.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libortp.9.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libosip2.7.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libosipparser2.7.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libspeex.1.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libspeexdsp.1.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libssl.1.0.0.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libswscale.2.1.100.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libtheora.0.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libtheora.0.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libvpx.1.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libz.1.dylib ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	
	COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/share/
	COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/share/images/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share/images/nowebcamCIF.jpg ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/share/images/
	COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/
	COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/
	COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share/sounds/linphone/ringback.wav ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share/sounds/linphone/rings/oldphone.wav ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share/sounds/linphone/rings/toy-mono.wav ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/
	
	# Change rpath
	COMMAND COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/mac_rpath.py ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/
	COMMAND COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/mac_rpath.py ${FB_BUNDLE_DIR}/${LINPHONEWEB_SHAREDIR}/ ${FB_BUNDLE_DIR}/${FBSTRING_PluginFileName}
)


###############################################################################
# XPI Package
if (NOT FB_XPI_PACKAGE_SUFFIX)
	SET(FB_XPI_PACKAGE_SUFFIX _XPI)
endif()
	
function (create_xpi_package PROJNAME PROJVERSION OUTDIR PROJDEP)
	SET(XPI_SOURCES
		${FB_OUT_DIR}/${FBSTRING_PluginFileName}.${PLUGIN_EXT}
		${CMAKE_CURRENT_BINARY_DIR}/install.rdf
		${CMAKE_CURRENT_SOURCE_DIR}/Mac/XPI/bootstrap.js
		${CMAKE_CURRENT_SOURCE_DIR}/Mac/XPI/chrome.manifest
		${CMAKE_CURRENT_SOURCE_DIR}/Common/icon48.png
		${CMAKE_CURRENT_SOURCE_DIR}/Common/icon64.png
	)
	
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Mac/XPI/install.rdf ${CMAKE_CURRENT_BINARY_DIR}/install.rdf)
	
	SET(FB_PKG_DIR ${FB_OUT_DIR}/XPI)
	GET_TARGET_PROPERTY(ONAME ${PROJNAME} OUTPUT_NAME)
	
	ADD_CUSTOM_TARGET(${PROJNAME}${FB_XPI_PACKAGE_SUFFIX} ALL DEPENDS ${OUTDIR}/${PROJNAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.xpi)
	SET_TARGET_PROPERTIES(${PROJNAME}${FB_XPI_PACKAGE_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
	ADD_CUSTOM_COMMAND(OUTPUT ${OUTDIR}/${PROJNAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.xpi
				 DEPENDS ${XPI_SOURCES}
				 COMMAND ${CMAKE_COMMAND} -E remove_directory ${FB_PKG_DIR}
				 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}
				 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/install.rdf ${FB_PKG_DIR}/
				 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Mac/XPI/bootstrap.js ${FB_PKG_DIR}/				 
				 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Mac/XPI/chrome.manifest ${FB_PKG_DIR}/
				 
				 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/chrome/skin
				 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Common/icon48.png ${FB_PKG_DIR}/chrome/skin/
				 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Common/icon64.png ${FB_PKG_DIR}/chrome/skin/
				 
				 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/plugins/
				 COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/copy.py ${FB_OUT_DIR}/${FBSTRING_PluginFileName}.${PLUGIN_EXT} ${FB_PKG_DIR}/plugins/${FBSTRING_PluginFileName}.${PLUGIN_EXT}
				 
				 COMMAND ${CMAKE_COMMAND} -E remove ${OUTDIR}/${PROJECT_NAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.xpi
				 COMMAND jar cfM ${OUTDIR}/${PROJNAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.xpi -C ${FB_PKG_DIR} .
				 COMMAND ${CMAKE_COMMAND} -E touch ${FB_OUT_DIR}/XPI.updated
	)
	ADD_DEPENDENCIES(${PROJNAME}${FB_XPI_PACKAGE_SUFFIX} ${PROJDEP})
	MESSAGE("-- Successfully added XPI package step")
endfunction(create_xpi_package)

create_xpi_package(${PLUGIN_NAME} ${FBSTRING_PLUGIN_VERSION} ${FB_OUT_DIR} ${PLUGIN_NAME})

create_signed_xpi(${PLUGIN_NAME} 
	"${FB_OUT_DIR}/XPI/"
	"${FB_OUT_DIR}/${PROJECT_NAME}-${FBSTRING_PLUGIN_VERSION}-${FB_PACKAGE_SUFFIX}.xpi"
	"${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pem"
	"${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
	${PLUGIN_NAME}${FB_XPI_PACKAGE_SUFFIX}
)
SET_TARGET_PROPERTIES(${PLUGIN_NAME}${FB_XPI_SIGNED_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
###############################################################################

###############################################################################
# CRX Package
if (NOT FB_CRX_PACKAGE_SUFFIX)
	SET(FB_CRX_PACKAGE_SUFFIX _CRX)
endif()
	
function (create_crx_package PROJNAME PROJVERSION OUTDIR PROJDEP)
	SET(CRX_SOURCES
		${FB_OUT_DIR}/${FBSTRING_PluginFileName}.${PLUGIN_EXT}
		${CMAKE_CURRENT_BINARY_DIR}/manifest.json
		${CMAKE_CURRENT_SOURCE_DIR}/Common/icon16.png
		${CMAKE_CURRENT_SOURCE_DIR}/Common/icon48.png
	)
	
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Mac/CRX/manifest.json ${CMAKE_CURRENT_BINARY_DIR}/manifest.json)
	
	SET(FB_PKG_DIR ${FB_OUT_DIR}/CRX)
	
	ADD_CUSTOM_TARGET(${PROJNAME}${FB_CRX_PACKAGE_SUFFIX} ALL DEPENDS ${OUTDIR}/${PROJECT_NAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.crx)
	SET_TARGET_PROPERTIES(${PROJNAME}${FB_CRX_PACKAGE_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
	ADD_CUSTOM_COMMAND(OUTPUT ${OUTDIR}/${PROJECT_NAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.crx
				 DEPENDS ${CRX_SOURCES}
				 COMMAND ${CMAKE_COMMAND} -E remove_directory ${FB_PKG_DIR}
				 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}
				 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/manifest.json ${FB_PKG_DIR}/
				 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Common/icon16.png ${FB_PKG_DIR}/
				 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Common/icon48.png ${FB_PKG_DIR}/
				 COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/copy.py ${FB_OUT_DIR}/${FBSTRING_PluginFileName}.${PLUGIN_EXT} ${FB_PKG_DIR}/${FBSTRING_PluginFileName}.${PLUGIN_EXT}

				 COMMAND ${CMAKE_COMMAND} -E remove ${OUTDIR}/${PROJECT_NAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.crx
				 COMMAND jar cfM ${OUTDIR}/${PROJECT_NAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.crx -C ${FB_PKG_DIR} .
				 COMMAND ${CMAKE_COMMAND} -E touch ${FB_OUT_DIR}/CRX.updated)
	ADD_DEPENDENCIES(${PROJNAME}${FB_CRX_PACKAGE_SUFFIX} ${PROJDEP})
	MESSAGE("-- Successfully added CRX package step")
endfunction(create_crx_package)

create_crx_package(${PLUGIN_NAME} ${FBSTRING_PLUGIN_VERSION} ${FB_OUT_DIR} ${PLUGIN_NAME})

create_signed_crx(${PLUGIN_NAME} 
	"${FB_OUT_DIR}/CRX/"
	"${FB_OUT_DIR}/${PROJECT_NAME}-${FBSTRING_PLUGIN_VERSION}-${FB_PACKAGE_SUFFIX}.crx"
	"${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pem"
	"${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
	${PLUGIN_NAME}${FB_CRX_PACKAGE_SUFFIX}
)
SET_TARGET_PROPERTIES(${PLUGIN_NAME}${FB_CRX_SIGNED_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
###############################################################################


###############################################################################
# PKG Package
if (NOT FB_PKG_PACKAGE_SUFFIX)
	SET(FB_PKG_PACKAGE_SUFFIX _PKG)
endif()

function (create_pkg_package PROJNAME PROJVERSION OUTDIR PROJDEP)
	SET(PKG_SOURCES ${FB_OUT_DIR}/${FBSTRING_PluginFileName}.${PLUGIN_EXT})
	
	FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Mac/PKG.pmdoc/move.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/PKG.pmdoc/)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Mac/PKG.pmdoc/index.xml ${CMAKE_CURRENT_BINARY_DIR}/PKG.pmdoc/index.xml)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Mac/PKG.pmdoc/01linphone.xml ${CMAKE_CURRENT_BINARY_DIR}/PKG.pmdoc/01linphone.xml)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Mac/PKG.pmdoc/01linphone-contents.xml ${CMAKE_CURRENT_BINARY_DIR}/PKG.pmdoc/01linphone-contents.xml)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Mac/PKG.pmdoc/move.sh ${CMAKE_CURRENT_BINARY_DIR}/PKG.pmdoc/move.sh @ONLY)
	
	ADD_CUSTOM_TARGET(${PROJNAME}${FB_PKG_PACKAGE_SUFFIX} ALL DEPENDS ${OUTDIR}/${PROJNAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}.pkg)
	SET_TARGET_PROPERTIES(${PLUGIN_NAME}${FB_PKG_PACKAGE_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
	ADD_CUSTOM_COMMAND(OUTPUT ${OUTDIR}/${PROJNAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}.pkg
				 DEPENDS ${PKG_SOURCES}
				 COMMAND ${CMAKE_COMMAND} -E remove_directory ${OUTDIR}/PKG.pmdoc
				 COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/copy.py ${CMAKE_CURRENT_BINARY_DIR}/PKG.pmdoc ${OUTDIR}/PKG.pmdoc
				 COMMAND /Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker --doc ${OUTDIR}/PKG.pmdoc --out ${OUTDIR}/${PROJNAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}.pkg
	)
	ADD_DEPENDENCIES(${PROJNAME}${FB_PKG_PACKAGE_SUFFIX} ${PROJDEP})
	MESSAGE("-- Successfully added PKG package step")
endfunction(create_pkg_package)

create_pkg_package(${PLUGIN_NAME} ${FBSTRING_PLUGIN_VERSION} ${FB_OUT_DIR} ${PLUGIN_NAME})
###############################################################################
