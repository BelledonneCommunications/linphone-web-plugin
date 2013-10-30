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

ADD_DEFINITIONS(
	-DPLUGIN_SHAREDIR="${PLUGIN_SHAREDIR}"
)

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
SET(DEPENDENCY_EXT "dylib")
GET_TARGET_PROPERTY(PLUGIN_EXT ${PROJECT_NAME} BUNDLE_EXTENSION)
SET(FB_PACKAGE_SUFFIX Mac)
IF(FB_PLATFORM_ARCH_64)
	SET(FB_PACKAGE_SUFFIX "${FB_PACKAGE_SUFFIX}-x86_64")
ELSE(FB_PLATFORM_ARCH_64)
	SET(FB_PACKAGE_SUFFIX "${FB_PACKAGE_SUFFIX}-x86")
ENDIF(FB_PLATFORM_ARCH_64)

SET(FB_OUT_DIR ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR})
SET(FB_BUNDLE_DIR ${FB_OUT_DIR}/${FBSTRING_PluginFileName}.${PLUGIN_EXT}/Contents/MacOS)

###############################################################################
# Get Core Rootfs tarball
if (NOT FB_CORE_ROOTFS_SUFFIX)
	SET(FB_CORE_ROOTFS_SUFFIX _CoreRootFS)
endif()

function (get_core_rootfs PROJNAME OUTDIR)
	SET(CORE_ROOTFS_GZTARBALL ${OUTDIR}/linphone-web-core-rootfs.tar.gz)
	if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs)
		FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs)
	endif()
	if (NOT EXISTS ${CORE_ROOTFS_GZTARBALL})
		message("-- Downloading core rootfs")
		FILE(DOWNLOAD ${CORE_ROOTFS_URL} ${CORE_ROOTFS_GZTARBALL} SHOW_PROGRESS STATUS CORE_ROOTFS_DL_STATUS)
		list(GET CORE_ROOTFS_DL_STATUS 0 CORE_ROOTFS_DL_STATUS_CODE)
		if (${CORE_ROOTFS_DL_STATUS_CODE} EQUAL 0)
			message("     Successful")
		else()
			list(GET CORE_ROOTFS_DL_STATUS 1 CORE_ROOTFS_DL_STATUS_MSG)
			message(FATAL_ERROR "     Failed: ${CORE_ROOTFS_DL_STATUS_CODE} ${CORE_ROOTFS_DL_STATUS_MSG}")
		endif()
	endif()

	ADD_CUSTOM_COMMAND(OUTPUT ${OUTDIR}/Rootfs.VERSION
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs
		DEPENDS ${CORE_ROOTFS_GZTARBALL}
		COMMAND ${CMAKE_COMMAND} -E remove -f *
		COMMAND ${CMAKE_COMMAND} -E tar xvzf ${CORE_ROOTFS_GZTARBALL}
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/VERSION ${OUTDIR}/Rootfs.VERSION
	)

	ADD_CUSTOM_TARGET(${PROJNAME}${FB_CORE_ROOTFS_SUFFIX} ALL DEPENDS ${OUTDIR}/Rootfs.VERSION)
	SET_TARGET_PROPERTIES(${PROJNAME}${FB_CORE_ROOTFS_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
	ADD_DEPENDENCIES(${PROJNAME} ${PROJNAME}${FB_CORE_ROOTFS_SUFFIX})
	MESSAGE("-- Successfully added Core Rootfs extration step")
endfunction(get_core_rootfs)
###############################################################################

get_core_rootfs(${PLUGIN_NAME} ${FB_BIN_DIR}/${PLUGIN_NAME})

###############################################################################
# Create Rootfs
if (NOT FB_ROOTFS_SUFFIX)
	SET(FB_ROOTFS_SUFFIX _RootFS)
endif()

function (create_rootfs PROJNAME OUTDIR)
	# Define components
	SET(ROOTFS_LIB_SOURCES
		liblinphone.5.${DEPENDENCY_EXT}
		libmediastreamer_base.3.${DEPENDENCY_EXT}
		libmediastreamer_voip.3.${DEPENDENCY_EXT}
		libogg.0.${DEPENDENCY_EXT}
		libopus.0.${DEPENDENCY_EXT}
		libortp.9.${DEPENDENCY_EXT}
		libspeex.1.${DEPENDENCY_EXT}
		libspeexdsp.1.${DEPENDENCY_EXT}
		libtheora.0.${DEPENDENCY_EXT}
		libtheora.0.${DEPENDENCY_EXT}
		libvpx.1.${DEPENDENCY_EXT}
		libz.1.${DEPENDENCY_EXT}
	)
	IF(LW_USE_SRTP)
		SET(ROOTFS_LIB_SOURCES
			${ROOTFS_LIB_SOURCES}
			libsrtp.1.4.5.${DEPENDENCY_EXT}
		)
	ENDIF(LW_USE_SRTP)
	IF(LW_USE_OPENSSL)
		SET(ROOTFS_LIB_SOURCES
			${ROOTFS_LIB_SOURCES}
			libcrypto.1.0.0.${DEPENDENCY_EXT}
			libssl.1.0.0.${DEPENDENCY_EXT}
		)
	ENDIF(LW_USE_OPENSSL)
	IF(LW_USE_EXOSIP)
		SET(ROOTFS_LIB_SOURCES
			${ROOTFS_LIB_SOURCES}
			libeXosip2.7.${DEPENDENCY_EXT}
			libosip2.7.${DEPENDENCY_EXT}
			libosipparser2.7.${DEPENDENCY_EXT}
		)
	ENDIF(LW_USE_EXOSIP)
	IF(LW_USE_FFMPEG)
		SET(ROOTFS_LIB_SOURCES
			${ROOTFS_LIB_SOURCES}
			libavcodec.53.61.100.${DEPENDENCY_EXT}
			libavutil.51.35.100.${DEPENDENCY_EXT}
			libswscale.2.1.100.${DEPENDENCY_EXT}
		)
	ENDIF(LW_USE_FFMPEG)
	IF(LW_USE_POLARSSL)
		SET(ROOTFS_LIB_SOURCES
			${ROOTFS_LIB_SOURCES}
			libpolarssl.0.${DEPENDENCY_EXT}
		)
	ENDIF(LW_USE_POLARSSL)
	IF(LW_USE_BELLESIP)
		SET(ROOTFS_LIB_SOURCES
			${ROOTFS_LIB_SOURCES}
			libbellesip.0.${DEPENDENCY_EXT}
			libantlr3c.0.${DEPENDENCY_EXT}
			libxml2.2.${DEPENDENCY_EXT}
		)
	ENDIF(LW_USE_BELLESIP)
	IF(LW_USE_G729)
		SET(ROOTFS_LIB_SOURCES
			${ROOTFS_LIB_SOURCES}
			libbcg729.0.${DEPENDENCY_EXT}
		)
		SET(ROOTFS_MS_PLUGINS_LIB_SOURCES
			${ROOTFS_MS_PLUGINS_LIB_SOURCES}
			libmsbcg729.0.${DEPENDENCY_EXT}
		)
	ENDIF(LW_USE_G729)

	SET(ROOTFS_SHARE_SOURCES
		linphone/rootca.pem
		images/nowebcamCIF.jpg
		sounds/linphone/ringback.wav
		sounds/linphone/rings/oldphone.wav
		sounds/linphone/rings/toy-mono.wav
	)

	# Set Rootfs sources
	SET(ROOTFS_SOURCES
		${OUTDIR}/${FBSTRING_PluginFileName}
	)
	FOREACH(elem ${ROOTFS_LIB_SOURCES})
		SET(DIR_SRC ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib)
		SET(DIR_DEST ${OUTDIR}/${PLUGIN_SHAREDIR})
		GET_FILENAME_COMPONENT(path ${elem} PATH)
		ADD_CUSTOM_COMMAND(OUTPUT ${DIR_DEST}/${elem} 
			DEPENDS ${DIR_SRC}/${elem}
			COMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_DEST}/${path}
			COMMAND ${CMAKE_COMMAND} -E copy ${DIR_SRC}/${elem} ${DIR_DEST}/${elem}
		)
		LIST(APPEND ROOTFS_SOURCES ${DIR_DEST}/${elem})
	ENDFOREACH(elem ${ROOTFS_LIB_SOURCES})
	FOREACH(elem ${ROOTFS_MS_PLUGINS_LIB_SOURCES})
		SET(DIR_SRC ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/mediastreamer/plugins)
		SET(DIR_DEST ${OUTDIR}/${PLUGIN_SHAREDIR}/lib/mediastreamer/plugins)
		GET_FILENAME_COMPONENT(path ${elem} PATH)
		ADD_CUSTOM_COMMAND(OUTPUT ${DIR_DEST}/${elem} 
			DEPENDS ${DIR_SRC}/${elem}
			COMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_DEST}/${path}
			COMMAND ${CMAKE_COMMAND} -E copy ${DIR_SRC}/${elem} ${DIR_DEST}/${elem}
		)
		LIST(APPEND ROOTFS_SOURCES ${DIR_DEST}/${elem})
	ENDFOREACH(elem ${ROOTFS_MS_PLUGINS_LIB_SOURCES})
	FOREACH(elem ${ROOTFS_SHARE_SOURCES})
		SET(DIR_SRC ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share)
		SET(DIR_DEST ${OUTDIR}/${PLUGIN_SHAREDIR}/share)
		GET_FILENAME_COMPONENT(path ${elem} PATH)
		ADD_CUSTOM_COMMAND(OUTPUT ${DIR_DEST}/${elem} 
			DEPENDS ${DIR_SRC}/${elem}
			COMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_DEST}/${path}
			COMMAND ${CMAKE_COMMAND} -E copy ${DIR_SRC}/${elem} ${DIR_DEST}/${elem}
		)
		LIST(APPEND ROOTFS_SOURCES ${DIR_DEST}/${elem})
	ENDFOREACH(elem ${ROOTFS_SHARE_SOURCES})


	ADD_CUSTOM_COMMAND(OUTPUT ${FB_OUT_DIR}/Rootfs.updated 
		DEPENDS ${ROOTFS_SOURCES}
		
		# Change rpath
		COMMAND COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/mac_rpath.py ${OUTDIR}/${PLUGIN_SHAREDIR}/
		COMMAND COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/mac_rpath.py ${OUTDIR}/${PLUGIN_SHAREDIR}/ ${OUTDIR}/${FBSTRING_PluginFileName}

		COMMAND ${CMAKE_COMMAND} -E touch ${FB_OUT_DIR}/Rootfs.updated
	)
	
	ADD_CUSTOM_TARGET(${PROJNAME}${FB_ROOTFS_SUFFIX} ALL DEPENDS ${FB_OUT_DIR}/Rootfs.updated)
	SET_TARGET_PROPERTIES(${PROJNAME}${FB_ROOTFS_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
	ADD_DEPENDENCIES(${PROJNAME}${FB_ROOTFS_SUFFIX} ${PROJNAME})
	MESSAGE("-- Successfully added Rootfs creation step")
endfunction(create_rootfs)
###############################################################################

create_rootfs(${PLUGIN_NAME} ${FB_BUNDLE_DIR})

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
				COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/crxmake.py ${FB_PKG_DIR} ${OUTDIR}/${PROJECT_NAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.crx
				COMMAND ${CMAKE_COMMAND} -E touch ${FB_OUT_DIR}/CRX.updated)
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
###############################################################################


###############################################################################
# PKG Package
if (NOT FB_PKG_PACKAGE_SUFFIX)
	SET(FB_PKG_PACKAGE_SUFFIX _PKG)
endif()

function (create_pkg_package PROJNAME PROJVERSION OUTDIR PROJDEP)
	SET(PKG_SOURCES ${FB_OUT_DIR}/${FBSTRING_PluginFileName}.${PLUGIN_EXT})

	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Mac/PKG.pmdoc/index.xml ${CMAKE_CURRENT_BINARY_DIR}/PKG.pmdoc/index.xml)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Mac/PKG.pmdoc/01linphone.xml ${CMAKE_CURRENT_BINARY_DIR}/PKG.pmdoc/01linphone.xml)
	CONFIGURE_FILE_EXT(${CMAKE_CURRENT_SOURCE_DIR}/Mac/PKG.pmdoc/01linphone-contents.xml ${CMAKE_CURRENT_BINARY_DIR}/PKG.pmdoc/01linphone-contents.xml)

	ADD_CUSTOM_TARGET(${PROJNAME}${FB_PKG_PACKAGE_SUFFIX} ALL DEPENDS ${OUTDIR}/${PROJNAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}.pkg)
	SET_TARGET_PROPERTIES(${PROJNAME}${FB_PKG_PACKAGE_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
	ADD_CUSTOM_COMMAND(OUTPUT ${OUTDIR}/${PROJNAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}.pkg
				DEPENDS ${PKG_SOURCES}
				COMMAND ${CMAKE_COMMAND} -E remove_directory ${OUTDIR}/PKG.pmdoc
				COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/copy.py ${CMAKE_CURRENT_BINARY_DIR}/PKG.pmdoc ${OUTDIR}/PKG.pmdoc
				COMMAND /Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker --doc ${OUTDIR}/PKG.pmdoc --out ${OUTDIR}/${PROJNAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}.pkg
	)
	ADD_DEPENDENCIES(${PROJNAME}${FB_PKG_PACKAGE_SUFFIX} ${PROJDEP})
	MESSAGE("-- Successfully added PKG package step")
endfunction(create_pkg_package)

create_pkg_package(${PLUGIN_NAME}
	${FBSTRING_PLUGIN_VERSION}
	${FB_OUT_DIR}
	${PLUGIN_NAME}${FB_ROOTFS_SUFFIX}
)
###############################################################################
