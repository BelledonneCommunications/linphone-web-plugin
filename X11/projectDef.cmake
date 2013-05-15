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
INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/Common/common.cmake)

# remember that the current source dir is the project root; this file is in X11/
FILE(GLOB PLATFORM RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	X11/[^.]*.cpp
	X11/[^.]*.h
	X11/[^.]*.cmake
)

# GCC options
ADD_DEFINITIONS(
	-std=c++0x
	-DCORE_THREADED
)

INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(Rootfs/include/linphone)
INCLUDE_DIRECTORIES(Rootfs/include)

SOURCE_GROUP(X11 FILES ${PLATFORM})

SET(SOURCES
	${SOURCES}
	${PLATFORM}
)

add_x11_plugin(${PROJECT_NAME} SOURCES)
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES FOLDER ${FBSTRING_ProductName})

# Add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	${PLUGIN_INTERNAL_DEPS}
	"${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/liblinphone.so.5"
	"${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libmediastreamer_base.so.3"
	"${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libmediastreamer_voip.so.3"
	"${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libortp.so.9"
)

SET(LINPHONEWEB_SHAREDIR linphoneweb)
SET(FB_PACKAGE_SUFFIX Linux)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	SET(FB_PACKAGE_SUFFIX "${FB_PACKAGE_SUFFIX}-x86_64")
else(CMAKE_SIZEOF_VOID_P EQUAL 8)
	SET(FB_PACKAGE_SUFFIX "${FB_PACKAGE_SUFFIX}-x86")
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

SET(PLUGIN_EXT "so")
SET(FB_OUT_DIR ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR})
SET(FB_ROOTFS_DIR ${FB_OUT_DIR}/Rootfs)

# Use default chrpath if not defined
IF(NOT DEFINED CMAKE_CHRPATH)
	SET(CMAKE_CHRPATH chrpath)
ENDIF(NOT DEFINED CMAKE_CHRPATH)

###############################################################################
# Create Rootfs
if (NOT FB_ROOTFS_SUFFIX)
	SET(FB_ROOTFS_SUFFIX _RootFS)
endif()

function (create_rootfs PROJNAME)
	SET(ROOTFS_SOURCES
		${FB_OUT_DIR}/${FBSTRING_PluginFileName}.so
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libavcodec.so.53
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libavutil.so.51
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libcrypto.so.1.0.0
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libeXosip2.so.7
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libjpeg.so.8
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/liblinphone.so.5
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libmediastreamer_base.so.3
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libmediastreamer_voip.so.3
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libogg.so.0
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libortp.so.9
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libosip2.so.7
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libosipparser2.so.7
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libspeex.so.1
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libspeexdsp.so.1
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libssl.so.1.0.0
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libswscale.so.2
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libtheora.so.0
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libv4l1.so.0
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libv4l2.so.0
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libv4lconvert.so.0
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libvpx.so.1
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libz.so.1
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share/images/nowebcamCIF.jpg
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share/sounds/linphone/ringback.wav
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share/sounds/linphone/rings/oldphone.wav
		${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share/sounds/linphone/rings/toy-mono.wav
	)
	
	ADD_CUSTOM_TARGET(${PROJNAME}${FB_ROOTFS_SUFFIX} ALL DEPENDS ${FB_ROOTFS_DIR})
	SET_TARGET_PROPERTIES(${PLUGIN_NAME}${FB_ROOTFS_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
	ADD_CUSTOM_COMMAND(OUTPUT ${FB_ROOTFS_DIR}
		DEPENDS ${ROOTFS_SOURCES}
		COMMAND ${CMAKE_COMMAND} -E remove_directory ${FB_ROOTFS_DIR}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_ROOTFS_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${FBSTRING_PluginFileName}.so ${FB_ROOTFS_DIR}/
		COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libavcodec.so.53 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libavutil.so.51 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libcrypto.so.1.0.0 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libeXosip2.so.7 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libjpeg.so.8 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/liblinphone.so.5 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libmediastreamer_base.so.3 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libmediastreamer_voip.so.3 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libogg.so.0 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libortp.so.9 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libosip2.so.7 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libosipparser2.so.7 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libspeex.so.1 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libspeexdsp.so.1 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libssl.so.1.0.0 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libswscale.so.2 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libtheora.so.0 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libv4l1.so.0 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libv4l2.so.0 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libv4lconvert.so.0 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libvpx.so.1 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libz.so.1 ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/

		COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/share/
		COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/share/linphone/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share/linphone/rootca.pem ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/share/linphone/
		COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/share/images/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share/images/nowebcamCIF.jpg ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/share/images/
		COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/
		COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/
		COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share/sounds/linphone/ringback.wav ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share/sounds/linphone/rings/oldphone.wav ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share/sounds/linphone/rings/toy-mono.wav ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/
		
		## Change rpath
		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/libavcodec.so.53
		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/libavutil.so.51
#		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/libcrypto.so.1.0.0
		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/libjpeg.so.8
		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/libeXosip2.so.7
		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/liblinphone.so.5
		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/libmediastreamer_base.so.3
		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/libmediastreamer_voip.so.3
		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/libogg.so.0
		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/libortp.so.9
		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/libosip2.so.7
		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/libosipparser2.so.7
		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/libspeex.so.1
		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/libspeexdsp.so.1
#		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/libssl.so.1.0.0
		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/libswscale.so.2
		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/libtheora.so.0
		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/libv4l1.so.0
		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/libv4l2.so.0
		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/libv4lconvert.so.0
		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/libvpx.so.1
		COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${FB_ROOTFS_DIR}/${LINPHONEWEB_SHAREDIR}/libz.so.1
		
		COMMAND ${CMAKE_COMMAND} -E touch ${FB_OUT_DIR}/Rootfs.updated
	)
	ADD_DEPENDENCIES(${PROJNAME}${FB_ROOTFS_SUFFIX} ${PROJNAME})
	MESSAGE("-- Successfully added Rootfs step")
endfunction(create_rootfs)
###############################################################################

create_rootfs(${PLUGIN_NAME})

# Add rpath to generated library
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,'\$ORIGIN/${LINPHONEWEB_SHAREDIR}'" )

###############################################################################
# TGZ Package
if (NOT FB_TGZ_PACKAGE_SUFFIX)
	SET(FB_TGZ_PACKAGE_SUFFIX _TGZ)
endif()

function (create_tgz_package PROJNAME PROJVERSION OUTDIR PROJDEP)
	SET(TGZ_SOURCES
		${FB_OUT_DIR}/Rootfs.updated
	)
	
	SET(FB_PKG_DIR ${FB_OUT_DIR}/TGZ)
	GET_TARGET_PROPERTY(ONAME ${PROJNAME} OUTPUT_NAME)
	
	SET(PKG_PREFIX ${PROJECT_NAME}-${FBSTRING_PLUGIN_VERSION})
	
	ADD_CUSTOM_TARGET(${PROJNAME}${FB_TGZ_PACKAGE_SUFFIX} ALL DEPENDS ${OUTDIR}/${PROJECT_NAME}-${FBSTRING_PLUGIN_VERSION}-${FB_PACKAGE_SUFFIX}.tar.gz)
	SET_TARGET_PROPERTIES(${PLUGIN_NAME}${FB_TGZ_PACKAGE_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
	ADD_CUSTOM_COMMAND(OUTPUT ${OUTDIR}/${PROJECT_NAME}-${FBSTRING_PLUGIN_VERSION}-${FB_PACKAGE_SUFFIX}.tar.gz
				DEPENDS ${TGZ_SOURCES}
				COMMAND ${CMAKE_COMMAND} -E remove_directory ${FB_PKG_DIR}
				COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}
				COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/copy.py ${FB_ROOTFS_DIR} ${FB_PKG_DIR}/${PKG_PREFIX}

				COMMAND ${CMAKE_COMMAND} -E remove ${OUTDIR}/${PROJECT_NAME}-${FBSTRING_PLUGIN_VERSION}-${FB_PACKAGE_SUFFIX}.tar.gz
				COMMAND tar zcvf ${OUTDIR}/${PROJECT_NAME}-${FBSTRING_PLUGIN_VERSION}-${FB_PACKAGE_SUFFIX}.tar.gz -C ${FB_PKG_DIR} ${PKG_PREFIX}
				
				COMMAND ${CMAKE_COMMAND} -E touch ${FB_OUT_DIR}/TGZ.updated
	)
	ADD_DEPENDENCIES(${PROJNAME}${FB_TGZ_PACKAGE_SUFFIX} ${PROJDEP})
	MESSAGE("-- Successfully added TGZ package step")
endfunction(create_tgz_package)

create_tgz_package(${PLUGIN_NAME}
	${FBSTRING_PLUGIN_VERSION}
	${FB_OUT_DIR}
	${PLUGIN_NAME}${FB_ROOTFS_SUFFIX}
)
###############################################################################

###############################################################################
# XPI Package
if (NOT FB_XPI_PACKAGE_SUFFIX)
	SET(FB_XPI_PACKAGE_SUFFIX _XPI)
endif()
	
function (create_xpi_package PROJNAME PROJVERSION OUTDIR PROJDEP)
	SET(XPI_SOURCES
		${FB_OUT_DIR}/Rootfs.updated
		${CMAKE_CURRENT_BINARY_DIR}/install.rdf
		${CMAKE_CURRENT_SOURCE_DIR}/X11/XPI/bootstrap.js
		${CMAKE_CURRENT_SOURCE_DIR}/X11/XPI/chrome.manifest
		${CMAKE_CURRENT_SOURCE_DIR}/Common/icon48.png
		${CMAKE_CURRENT_SOURCE_DIR}/Common/icon64.png
	)
	
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/X11/XPI/install.rdf ${CMAKE_CURRENT_BINARY_DIR}/install.rdf)
	
	SET(FB_PKG_DIR ${FB_OUT_DIR}/XPI)
	GET_TARGET_PROPERTY(ONAME ${PROJNAME} OUTPUT_NAME)
	
	ADD_CUSTOM_TARGET(${PROJNAME}${FB_XPI_PACKAGE_SUFFIX} ALL DEPENDS ${OUTDIR}/${PROJNAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.xpi)
	SET_TARGET_PROPERTIES(${PROJNAME}${FB_XPI_PACKAGE_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
	ADD_CUSTOM_COMMAND(OUTPUT ${OUTDIR}/${PROJNAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.xpi
				DEPENDS ${XPI_SOURCES}
				COMMAND ${CMAKE_COMMAND} -E remove_directory ${FB_PKG_DIR}
				COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/install.rdf ${FB_PKG_DIR}/
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/X11/XPI/bootstrap.js ${FB_PKG_DIR}/
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/X11/XPI/chrome.manifest ${FB_PKG_DIR}/
				
				COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/chrome/skin
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Common/icon48.png ${FB_PKG_DIR}/chrome/skin/
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Common/icon64.png ${FB_PKG_DIR}/chrome/skin/
				
				COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/copy.py ${FB_ROOTFS_DIR} ${FB_PKG_DIR}/plugins

				COMMAND ${CMAKE_COMMAND} -E remove ${OUTDIR}/${PROJNAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.xpi
				COMMAND jar cfM ${OUTDIR}/${PROJNAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.xpi -C ${FB_PKG_DIR} .
				
				COMMAND ${CMAKE_COMMAND} -E touch ${FB_OUT_DIR}/XPI.updated
	)
	ADD_DEPENDENCIES(${PROJNAME}${FB_XPI_PACKAGE_SUFFIX} ${PROJDEP})
	MESSAGE("-- Successfully added XPI package step")
endfunction(create_xpi_package)

create_xpi_package(${PLUGIN_NAME}
	${FBSTRING_PLUGIN_VERSION}
	${FB_OUT_DIR}
	${PLUGIN_NAME}${FB_ROOTFS_SUFFIX}
)

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
		${FB_OUT_DIR}/Rootfs.updated
		${CMAKE_CURRENT_BINARY_DIR}/manifest.json
		${CMAKE_CURRENT_SOURCE_DIR}/Common/icon16.png
		${CMAKE_CURRENT_SOURCE_DIR}/Common/icon48.png
	)
	
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/X11/CRX/manifest.json ${CMAKE_CURRENT_BINARY_DIR}/manifest.json)
	
	SET(FB_PKG_DIR ${FB_OUT_DIR}/CRX)
	
	ADD_CUSTOM_TARGET(${PROJNAME}${FB_CRX_PACKAGE_SUFFIX} ALL DEPENDS ${OUTDIR}/${PROJECT_NAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.crx)
	SET_TARGET_PROPERTIES(${PROJNAME}${FB_CRX_PACKAGE_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
	ADD_CUSTOM_COMMAND(OUTPUT ${OUTDIR}/${PROJECT_NAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.crx
				DEPENDS ${CRX_SOURCES}
				COMMAND ${CMAKE_COMMAND} -E remove_directory ${FB_PKG_DIR}
				COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/copy.py ${FB_ROOTFS_DIR} ${FB_PKG_DIR}
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/manifest.json ${FB_PKG_DIR}/
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Common/icon16.png ${FB_PKG_DIR}/
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Common/icon48.png ${FB_PKG_DIR}/
					
				COMMAND ${CMAKE_COMMAND} -E remove ${OUTDIR}/${PROJECT_NAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.crx
				COMMAND jar cfM ${OUTDIR}/${PROJECT_NAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.crx -C ${FB_PKG_DIR} .
				
				COMMAND ${CMAKE_COMMAND} -E touch ${FB_OUT_DIR}/CRX.updated
	)
	ADD_DEPENDENCIES(${PROJNAME}${FB_CRX_PACKAGE_SUFFIX} ${PROJDEP})
	MESSAGE("-- Successfully added CRX package step")
endfunction(create_crx_package)

create_crx_package(${PLUGIN_NAME}
	${FBSTRING_PLUGIN_VERSION}
	${FB_OUT_DIR}
	${PLUGIN_NAME}${FB_ROOTFS_SUFFIX}
)

create_signed_crx(${PLUGIN_NAME}
	"${FB_OUT_DIR}/CRX/"
	"${FB_OUT_DIR}/${PROJECT_NAME}-${FBSTRING_PLUGIN_VERSION}-${FB_PACKAGE_SUFFIX}.crx"
	"${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pem"
	"${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
	${PLUGIN_NAME}${FB_CRX_PACKAGE_SUFFIX}
)
SET_TARGET_PROPERTIES(${PLUGIN_NAME}${FB_CRX_SIGNED_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
###############################################################################
