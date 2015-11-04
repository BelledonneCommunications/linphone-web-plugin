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

# Mac template platform definition CMake file
# Included from ../CMakeLists.txt
INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/Common/common.cmake)

set(LW_PASSPHRASE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/sign/${LW_PASSPHRASE_FILENAME}")

# remember that the current source dir is the project root; this file is in Mac/
FILE(GLOB PLATFORM RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	Mac/[^.]*.cpp
	Mac/[^.]*.mm
	Mac/[^.]*.h
	Mac/[^.]*.cmake
)

ADD_DEFINITIONS(
	-DFBSTRING_MIMEType_Video="${FBSTRING_MIMEType_Video}"
	-DPLUGIN_SHAREDIR="${PLUGIN_SHAREDIR}"
)

SOURCE_GROUP(Mac FILES ${PLATFORM})

SET(SOURCES
	${SOURCES}
	${PLATFORM})

INCLUDE_DIRECTORIES(${CMAKE_INSTALL_PREFIX}/include)

SET(PLIST "Mac/bundle_template/Info.plist")
SET(STRINGS "Mac/bundle_template/InfoPlist.strings")
SET(LOCALIZED "Mac/bundle_template/Localized.r")

add_mac_plugin(${PROJECT_NAME} ${PLIST} ${STRINGS} ${LOCALIZED} SOURCES)
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES FOLDER ${FBSTRING_ProductName})

FIND_LIBRARY(OPENGL_FRAMEWORK OpenGL)
FIND_LIBRARY(QUARTZ_CORE_FRAMEWORK QuartzCore)

# add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!
SET(DEPENDENCY_EXT "dylib")
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		${PLUGIN_INTERNAL_DEPS}
		${CMAKE_INSTALL_PREFIX}/lib/liblinphone.8.${DEPENDENCY_EXT}
		${CMAKE_INSTALL_PREFIX}/lib/libmediastreamer_base.6.${DEPENDENCY_EXT}
		${CMAKE_INSTALL_PREFIX}/lib/libmediastreamer_voip.6.${DEPENDENCY_EXT}
		${CMAKE_INSTALL_PREFIX}/lib/libortp.10.${DEPENDENCY_EXT}
		${OPENGL_FRAMEWORK}
		${QUARTZ_CORE_FRAMEWORK}
)
add_dependencies(${PROJECT_NAME} TARGET_linphone_builder)


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
IF(FB_PLATFORM_ARCH_64)
	SET(FB_PACKAGE_SUFFIX "${FB_PACKAGE_SUFFIX}-x86_64")
ELSE(FB_PLATFORM_ARCH_64)
	SET(FB_PACKAGE_SUFFIX "${FB_PACKAGE_SUFFIX}-x86")
ENDIF(FB_PLATFORM_ARCH_64)

SET(FB_OUT_DIR ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR})
SET(FB_BUNDLE_DIR ${FB_OUT_DIR}/${FBSTRING_PluginFileName}.${PLUGIN_EXT}/Contents/MacOS)

# Configuration of code signing
find_program(MAC_CERTTOOL certtool)
find_program(MAC_CODESIGN codesign)
if(NOT "${MAC_CERTTOOL}" STREQUAL "" AND NOT "${MAC_CODESIGN}" STREQUAL "")
	execute_process(COMMAND ${MAC_CERTTOOL} y
			OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/certtool_output)
	execute_process(COMMAND grep "${MAC_CODE_SIGNING_IDENTITY}"
			INPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/certtool_output
			OUTPUT_VARIABLE CODE_SIGNING_IDENTITY_FOUND)
	execute_process(COMMAND grep "${MAC_INSTALLER_SIGNING_IDENTITY}"
			INPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/certtool_output
			OUTPUT_VARIABLE INSTALLER_SIGNING_IDENTITY_FOUND)
	if ("${CODE_SIGNING_IDENTITY_FOUND}" STREQUAL "")
		message(STATUS "Code signing identity \"${MAC_CODE_SIGNING_IDENTITY}\" not found! Can not sign code.")
	else()
		set(LW_CODESIGN_COMMAND "${MAC_CODESIGN}" -s "${MAC_CODE_SIGNING_IDENTITY}")
	endif()
	if ("${INSTALLER_SIGNING_IDENTITY_FOUND}" STREQUAL "")
		message(STATUS "Installer signing identity \"${MAC_INSTALLER_SIGNING_IDENTITY}\" not found! Can not sign installer.")
	endif()
else()
	message(STATUS "certtool or codesign utilities not found! Can not sign code or installer.")
endif()

###############################################################################
# Create Rootfs
if (NOT FB_ROOTFS_SUFFIX)
	SET(FB_ROOTFS_SUFFIX _RootFS)
endif()
if (NOT FB_RPATH_SUFFIX)
	SET(FB_RPATH_SUFFIX _Rpath)
endif()

set(INSTALL_ROOTFS_TARGET_ID 0)
set(INSTALL_ROOTFS_TARGETS )
macro(install_rootfs_target VAR_DEPEND DIR_DEST DIR_SRC elem)
	get_filename_component(path ${elem} PATH)
	add_custom_target("INSTALL_ROOTFS_TARGET_${INSTALL_ROOTFS_TARGET_ID}"
		COMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_DEST}/${path}
		COMMAND ${CMAKE_COMMAND} -E copy ${DIR_SRC}/${elem} ${DIR_DEST}/${elem}
		COMMENT "Install ${DIR_DEST}/${elem}"
		VERBATIM
	)
	set_target_properties("INSTALL_ROOTFS_TARGET_${INSTALL_ROOTFS_TARGET_ID}" PROPERTIES FOLDER ${FBSTRING_ProductName})
	add_dependencies("INSTALL_ROOTFS_TARGET_${INSTALL_ROOTFS_TARGET_ID}" ${VAR_DEPEND})
	list(APPEND INSTALL_ROOTFS_TARGETS "INSTALL_ROOTFS_TARGET_${INSTALL_ROOTFS_TARGET_ID}")
	math(EXPR INSTALL_ROOTFS_TARGET_ID "${INSTALL_ROOTFS_TARGET_ID}+1")
endmacro()
set(SIGN_ROOTFS_TARGET_ID 0)
set(SIGN_ROOTFS_TARGETS )
macro(sign_rootfs_target VAR_DEPEND DIR_DEST elem)
	add_custom_target("SIGN_ROOTFS_TARGET_${SIGN_ROOTFS_TARGET_ID}"
		COMMAND ${LW_CODESIGN_COMMAND} -v ${DIR_DEST}/${elem}
		COMMENT "Sign ${DIR_DEST}/${elem}"
		VERBATIM
	)
	set_target_properties("SIGN_ROOTFS_TARGET_${SIGN_ROOTFS_TARGET_ID}" PROPERTIES FOLDER ${FBSTRING_ProductName})
	add_dependencies("SIGN_ROOTFS_TARGET_${SIGN_ROOTFS_TARGET_ID}" ${VAR_DEPEND})
	list(APPEND SIGN_ROOTFS_TARGETS "SIGN_ROOTFS_TARGET_${SIGN_ROOTFS_TARGET_ID}")
	math(EXPR SIGN_ROOTFS_TARGET_ID "${SIGN_ROOTFS_TARGET_ID}+1")
endmacro()

function (create_rootfs PROJNAME OUTDIR)
	# Define components
	set(ROOTFS_MS_PLUGINS_LIB_SOURCES )
	set(ROOTFS_LIB_SOURCES
		libantlr3c.1.${DEPENDENCY_EXT}
		libbellesip.0.${DEPENDENCY_EXT}
		liblinphone.8.${DEPENDENCY_EXT}
		libmediastreamer_base.6.${DEPENDENCY_EXT}
		libmediastreamer_voip.6.${DEPENDENCY_EXT}
		libortp.10.${DEPENDENCY_EXT}
		libpolarssl.8.${DEPENDENCY_EXT}
		libspeex.6.${DEPENDENCY_EXT}
		libspeexdsp.6.${DEPENDENCY_EXT}
		libz.1.${DEPENDENCY_EXT}
	)
	if(ENABLE_SRTP)
		list(APPEND ROOTFS_LIB_SOURCES libsrtp.1.${DEPENDENCY_EXT})
	endif()
	if(ENABLE_ZRTP)
		list(APPEND ROOTFS_LIB_SOURCES libbzrtp.0.${DEPENDENCY_EXT})
	endif()
	if(ENABLE_FFMPEG)
		list(APPEND ROOTFS_LIB_SOURCES
			libavcodec.53.61.100.${DEPENDENCY_EXT}
			libavutil.51.35.100.${DEPENDENCY_EXT}
			libswscale.2.1.100.${DEPENDENCY_EXT}
		)
	endif()
	if(ENABLE_TUNNEL)
		list(APPEND ROOTFS_LIB_SOURCES libtunnel.0.${DEPENDENCY_EXT})
	endif()
	if(ENABLE_AMR)
		list(APPEND ROOTFS_MS_PLUGINS_LIB_SOURCES libmsamr.0.${DEPENDENCY_EXT})
	endif()
	if(ENABLE_G729)
		list(APPEND ROOTFS_MS_PLUGINS_LIB_SOURCES libmsbcg729.0.${DEPENDENCY_EXT})
	endif()
	if(ENABLE_ILBC)
		list(APPEND ROOTFS_MS_PLUGINS_LIB_SOURCES libmsilbc.0.${DEPENDENCY_EXT})
	endif()
	if(ENABLE_OPENH264)
		list(APPEND ROOTFS_MS_PLUGINS_LIB_SOURCES libmsopenh264.0.${DEPENDENCY_EXT})
	endif()
	if(ENABLE_X264)
		list(APPEND ROOTFS_MS_PLUGINS_LIB_SOURCES libmsx264.0.${DEPENDENCY_EXT})
	endif()

	set(ROOTFS_SHARE_SOURCES
		linphone/rootca.pem
		images/nowebcamCIF.jpg
		sounds/linphone/ringback.wav
		sounds/linphone/rings/oldphone.wav
		sounds/linphone/rings/toy-mono.wav
	)

	foreach(elem ${ROOTFS_LIB_SOURCES})
		install_rootfs_target(
			${PROJNAME}
			${OUTDIR}/${PLUGIN_SHAREDIR}
			${CMAKE_INSTALL_PREFIX}/lib
			${elem}
		)
	endforeach()
	foreach(elem ${ROOTFS_MS_PLUGINS_LIB_SOURCES})
		install_rootfs_target(
			${PROJNAME}
			${OUTDIR}/${PLUGIN_SHAREDIR}/lib/mediastreamer/plugins
			${CMAKE_INSTALL_PREFIX}/lib/mediastreamer/plugins
			${elem}
		)
	endforeach()
	foreach(elem ${ROOTFS_SHARE_SOURCES})
		install_rootfs_target(
			${PROJNAME}
			${OUTDIR}/${PLUGIN_SHAREDIR}/share
			${CMAKE_INSTALL_PREFIX}/share
			${elem}
		)
	endforeach()

	# Change rpath
	add_custom_target(${PROJNAME}${FB_RPATH_SUFFIX} ALL
		DEPENDS ${INSTALL_ROOTFS_TARGETS}
		COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Common/mac_rpath.py ${OUTDIR}/${PLUGIN_SHAREDIR}/
		COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Common/mac_rpath.py ${OUTDIR}/${PLUGIN_SHAREDIR}/ ${OUTDIR}/${FBSTRING_PluginFileName}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTDIR}/${PLUGIN_SHAREDIR}/lib/mediastreamer/plugins
		COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Common/mac_rpath.py ${OUTDIR}/${PLUGIN_SHAREDIR}/ ${OUTDIR}/${PLUGIN_SHAREDIR}/lib/mediastreamer/plugins/
	)

	foreach(elem ${ROOTFS_LIB_SOURCES})
		sign_rootfs_target(
			${PROJNAME}${FB_RPATH_SUFFIX}
			${OUTDIR}/${PLUGIN_SHAREDIR}
			${elem}
		)
	endforeach()
	foreach(elem ${ROOTFS_MS_PLUGINS_LIB_SOURCES})
		sign_rootfs_target(
			${PROJNAME}${FB_RPATH_SUFFIX}
			${OUTDIR}/${PLUGIN_SHAREDIR}/lib/mediastreamer/plugins
			${elem}
		)
	endforeach()
	foreach(elem ${ROOTFS_SHARE_SOURCES})
		sign_rootfs_target(
			${PROJNAME}${FB_RPATH_SUFFIX}
			${OUTDIR}/${PLUGIN_SHAREDIR}/share
			${elem}
		)
	endforeach()

	add_custom_target(${PROJNAME}${FB_ROOTFS_SUFFIX} ALL
		DEPENDS ${SIGN_ROOTFS_TARGETS}
		COMMAND ${LW_CODESIGN_COMMAND} -v ${OUTDIR}/${FBSTRING_PluginFileName}

	)
	add_dependencies(${PROJNAME}${FB_ROOTFS_SUFFIX} ${PROJNAME})
	message("-- Successfully added Rootfs creation step")
endfunction()
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
		${CMAKE_CURRENT_BINARY_DIR}/chrome.manifest
		${CMAKE_CURRENT_SOURCE_DIR}/Common/icon48.png
		${CMAKE_CURRENT_SOURCE_DIR}/Common/icon64.png
	)

	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Mac/XPI/install.rdf ${CMAKE_CURRENT_BINARY_DIR}/install.rdf)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Mac/XPI/chrome.manifest ${CMAKE_CURRENT_BINARY_DIR}/chrome.manifest)

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
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/chrome.manifest ${FB_PKG_DIR}/

				COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/chrome/skin
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Common/icon48.png ${FB_PKG_DIR}/chrome/skin/
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Common/icon64.png ${FB_PKG_DIR}/chrome/skin/

				COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/plugins/
				COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Common/copy.py ${FB_OUT_DIR}/${FBSTRING_PluginFileName}.${PLUGIN_EXT} ${FB_PKG_DIR}/plugins/${FBSTRING_PluginFileName}.${PLUGIN_EXT}

				COMMAND ${CMAKE_COMMAND} -E remove ${OUTDIR}/${PROJECT_NAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.xpi
				COMMAND ${Java_JAR_EXECUTABLE} cfM ${OUTDIR}/${PROJNAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.xpi -C ${FB_PKG_DIR} .
				COMMAND ${CMAKE_COMMAND} -E touch ${FB_OUT_DIR}/XPI.updated
	)
	ADD_DEPENDENCIES(${PROJNAME}${FB_XPI_PACKAGE_SUFFIX} ${PROJDEP})
	MESSAGE("-- Successfully added XPI package step")
endfunction(create_xpi_package)

create_xpi_package(${PLUGIN_NAME}
	${FBSTRING_PLUGIN_GIT_REVISION}
	${FB_OUT_DIR}
	${PLUGIN_NAME}${FB_ROOTFS_SUFFIX}
)

create_signed_xpi(${PLUGIN_NAME}
	"${FB_OUT_DIR}/XPI/"
	"${FB_OUT_DIR}/${PROJECT_NAME}-${FBSTRING_PLUGIN_GIT_REVISION}-${FB_PACKAGE_SUFFIX}.xpi"
	"${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pem"
	"${LW_PASSPHRASE_FILE}"
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
				COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Common/copy.py ${FB_OUT_DIR}/${FBSTRING_PluginFileName}.${PLUGIN_EXT} ${FB_PKG_DIR}/${FBSTRING_PluginFileName}.${PLUGIN_EXT}

				COMMAND ${CMAKE_COMMAND} -E remove ${OUTDIR}/${PROJECT_NAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.crx
				COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Common/crxmake.py ${FB_PKG_DIR} ${OUTDIR}/${PROJECT_NAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.crx
				COMMAND ${CMAKE_COMMAND} -E touch ${FB_OUT_DIR}/CRX.updated)
	ADD_DEPENDENCIES(${PROJNAME}${FB_CRX_PACKAGE_SUFFIX} ${PROJDEP})
	MESSAGE("-- Successfully added CRX package step")
endfunction(create_crx_package)

create_crx_package(${PLUGIN_NAME}
	${FBSTRING_PLUGIN_GIT_REVISION}
	${FB_OUT_DIR}
	${PLUGIN_NAME}${FB_ROOTFS_SUFFIX}
)

create_signed_crx(${PLUGIN_NAME}
	"${FB_OUT_DIR}/CRX/"
	"${FB_OUT_DIR}/${PROJECT_NAME}-${FBSTRING_PLUGIN_GIT_REVISION}-${FB_PACKAGE_SUFFIX}.crx"
	"${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pem"
	"${LW_PASSPHRASE_FILE}"
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
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Mac/PKG.pmdoc/scripts/remove-old-install.sh ${CMAKE_CURRENT_BINARY_DIR}/PKG.pmdoc/scripts/remove-old-install.sh @ONLY)

	ADD_CUSTOM_TARGET(${PROJNAME}${FB_PKG_PACKAGE_SUFFIX} ALL DEPENDS ${OUTDIR}/${PROJNAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}.pkg)
	SET_TARGET_PROPERTIES(${PROJNAME}${FB_PKG_PACKAGE_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
	ADD_CUSTOM_COMMAND(OUTPUT ${OUTDIR}/${PROJNAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.pkg
				DEPENDS ${PKG_SOURCES}
				COMMAND ${CMAKE_COMMAND} -E remove_directory ${OUTDIR}/PKG.pmdoc
				COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Common/copy.py ${CMAKE_CURRENT_BINARY_DIR}/PKG.pmdoc ${OUTDIR}/PKG.pmdoc
				COMMAND /Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker --doc ${OUTDIR}/PKG.pmdoc --out ${OUTDIR}/${PROJNAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.pkg
	)
	ADD_DEPENDENCIES(${PROJNAME}${FB_PKG_PACKAGE_SUFFIX} ${PROJDEP})
	MESSAGE("-- Successfully added PKG package step")
endfunction(create_pkg_package)

create_pkg_package(${PLUGIN_NAME}
	${FBSTRING_PLUGIN_GIT_REVISION}
	${FB_OUT_DIR}
	${PLUGIN_NAME}${FB_ROOTFS_SUFFIX}
)

if (NOT "${INSTALLER_SIGNING_IDENTITY_FOUND}" STREQUAL "")
	create_signed_pkg(${PLUGIN_NAME}
		"${FB_OUT_DIR}/${PROJECT_NAME}-${FBSTRING_PLUGIN_GIT_REVISION}-${FB_PACKAGE_SUFFIX}-unsigned.pkg"
		"${FB_OUT_DIR}/${PROJECT_NAME}-${FBSTRING_PLUGIN_GIT_REVISION}-${FB_PACKAGE_SUFFIX}.pkg"
		${MAC_INSTALLER_SIGNING_IDENTITY}
		${PLUGIN_NAME}${FB_PKG_PACKAGE_SUFFIX}
	)
endif()
###############################################################################
