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

# Windows template platform definition CMake file
# Included from ../CMakeLists.txt
INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/Common/common.cmake)

option(LW_CREATE_MSI "Enable creation of a MSI package of linphoneweb" ON)
option(LW_CREATE_CAB "Enable creation of a CAB package of linphoneweb" ON)

# Configuration of code signing
set(LW_PFX_FILE "${CMAKE_CURRENT_SOURCE_DIR}/sign/${LW_PFX_FILENAME}")
set(LW_PASSPHRASE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/sign/${LW_PASSPHRASE_FILENAME}")
GET_FILENAME_COMPONENT(WINSDK_DIR "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows;CurrentInstallFolder]" REALPATH CACHE)
find_program(SIGNTOOL signtool
	PATHS
	${WINSDK_DIR}/bin
)
set(LW_SIGNTOOL_COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Common/signtool.py)
if(EXISTS ${LW_PFX_FILE})
	if(SIGNTOOL)
		set(LW_SIGNTOOL_COMMAND ${LW_SIGNTOOL_COMMAND} signtool sign /f "${LW_PFX_FILE}")
		if (NOT "${LW_PASSPHRASE_FILE}" STREQUAL "")
			set(LW_SIGNTOOL_COMMAND ${LW_SIGNTOOL_COMMAND} /p ${LW_PASSPHRASE_FILE})
		endif()
		if (NOT "${LW_TIMESTAMP_URL}" STREQUAL "")
			set(LW_SIGNTOOL_COMMAND ${LW_SIGNTOOL_COMMAND} /t "${LW_TIMESTAMP_URL}")
		endif()
		message("Found signtool and certificate ${LW_PFX_FILE}")
	else(SIGNTOOL)
		message("!! Could not find signtool! Code signing disabled ${SIGNTOOL}")
	endif(SIGNTOOL)
else(EXISTS ${LW_PFX_FILE})
	message(STATUS "No signtool certificate found; assuming development machine (${LW_PFX_FILE})")
endif(EXISTS ${LW_PFX_FILE})
macro(my_sign_file PROJNAME _FILENAME)
	if (EXISTS ${LW_PFX_FILE} AND SIGNTOOL)
		ADD_CUSTOM_COMMAND(
			TARGET ${PROJNAME}
			POST_BUILD
			COMMAND ${LW_SIGNTOOL_COMMAND} ${_FILENAME}
		)
		message(STATUS "Successfully added signtool step to sign ${_FILENAME} with ${LW_PFX_FILE}")
	endif()
endmacro(my_sign_file)

# remember that the current source dir is the project root; this file is in Win/
FILE(GLOB PLATFORM RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	Win/[^.]*.cpp
	Win/[^.]*.h
	Win/[^.]*.cmake
)

# use this to add preprocessor definitions
ADD_DEFINITIONS(
	/D "FBSTRING_MIMEType_Video=\\\"${FBSTRING_MIMEType_Video}\\\""
	/D "PLUGIN_SHAREDIR=\\\"${PLUGIN_SHAREDIR}\\\""
	/D "_ATL_STATIC_REGISTRY"
	/wd4355
)
if(LW_DEBUG_GENERATE_DUMPS)
	add_definitions(-DLW_DEBUG_GENERATE_DUMPS)
endif(LW_DEBUG_GENERATE_DUMPS)

SOURCE_GROUP(Win FILES ${PLATFORM})

SET(SOURCES
	${SOURCES}
	${PLATFORM}
)

SET(CMAKE_SHARED_LINKER_FLAGS_RELEASE
		"/LTCG /SUBSYSTEM:WINDOWS /OPT:NOREF /OPT:ICF")
SET(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL
		"/LTCG /SUBSYSTEM:WINDOWS /OPT:NOREF /OPT:ICF")
SET(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO
		"/LTCG /SUBSYSTEM:WINDOWS /OPT:NOREF /OPT:ICF")

INCLUDE_DIRECTORIES(${CMAKE_INSTALL_PREFIX}/include)

add_windows_plugin(${PROJECT_NAME} SOURCES)
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES FOLDER ${FBSTRING_ProductName})

# add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	${PLUGIN_INTERNAL_DEPS}
	${CMAKE_INSTALL_PREFIX}/lib/linphone.lib
	${CMAKE_INSTALL_PREFIX}/lib/mediastreamer_base.lib
	${CMAKE_INSTALL_PREFIX}/lib/mediastreamer_voip.lib
	${CMAKE_INSTALL_PREFIX}/lib/ortp.lib
)
add_dependencies(${PROJECT_NAME} TARGET_linphone_builder)

SET(FB_PACKAGE_SUFFIX Win)
IF(FB_PLATFORM_ARCH_64)
	SET(FB_PACKAGE_SUFFIX "${FB_PACKAGE_SUFFIX}64")
ELSE(FB_PLATFORM_ARCH_64)
	SET(FB_PACKAGE_SUFFIX "${FB_PACKAGE_SUFFIX}32")
ENDIF(FB_PLATFORM_ARCH_64)

SET(DEPENDENCY_EXT "dll")
SET(PLUGIN_EXT "dll")
SET(FB_OUT_DIR ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR})
SET(FB_ROOTFS_DIR ${FB_OUT_DIR}/Rootfs)
SET(WIX_LINK_FLAGS -dConfiguration=${CMAKE_CFG_INTDIR})

if(WITH_DYNAMIC_MSVC_RUNTIME)
	string(REGEX REPLACE "Visual Studio ([0-9]+).*" "\\1" MSVC_VERSION "${CMAKE_GENERATOR}")
	if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		set(MSVC_DEBUG_SYSTEM_LIBRARIES "d")
	endif()
	find_file(MSVCP_LIB msvcp${MSVC_VERSION}0${MSVC_DEBUG_SYSTEM_LIBRARIES}.dll PATHS "C:/Windows/System32")
	find_file(MSVCR_LIB msvcr${MSVC_VERSION}0${MSVC_DEBUG_SYSTEM_LIBRARIES}.dll PATHS "C:/Windows/System32")
	if(NOT MSVCP_LIB OR NOT MSVCR_LIB)
		message(FATAL_ERROR "You need to install the Visual Studio C++ Redistributable libraries!")
	endif()
endif()

###############################################################################
# Create Rootfs
if (NOT FB_ROOTFS_SUFFIX)
	SET(FB_ROOTFS_SUFFIX _RootFS)
endif()

set(ROOTFS_TARGET_ID 0)
set(ROOTFS_TARGETS )
macro(add_rootfs_target VAR_DEPEND DIR_DEST DIR_SRC elem SIGN)
	get_filename_component(path ${elem} PATH)
	if(${SIGN})
		get_filename_component(elem_we ${elem} NAME_WE)
		add_custom_target("ROOTFS_TARGET_${ROOTFS_TARGET_ID}"
			COMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_DEST}/${path}
			COMMAND ${CMAKE_COMMAND} -E copy ${DIR_SRC}/${elem} ${DIR_DEST}/${elem}
			COMMAND ${CMAKE_COMMAND} -E copy ${DIR_SRC}/${elem_we}.pdb ${DIR_DEST}/${elem_we}.pdb || ${CMAKE_COMMAND} -E echo "Missing ${DIR_SRC}/${elem_we}.pdb"
			COMMAND ${LW_SIGNTOOL_COMMAND} ${DIR_DEST}/${elem}
			COMMENT "Install and sign ${DIR_DEST}/${elem}"
			VERBATIM
		)
	else(${SIGN})
		add_custom_target("ROOTFS_TARGET_${ROOTFS_TARGET_ID}"
			COMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_DEST}/${path}
			COMMAND ${CMAKE_COMMAND} -E copy ${DIR_SRC}/${elem} ${DIR_DEST}/${elem}
			COMMENT "Install ${DIR_DEST}/${elem}"
			VERBATIM
		)
	endif(${SIGN})
	set_target_properties("ROOTFS_TARGET_${ROOTFS_TARGET_ID}" PROPERTIES FOLDER ${FBSTRING_ProductName})
	add_dependencies("ROOTFS_TARGET_${ROOTFS_TARGET_ID}" ${VAR_DEPEND})
	list(APPEND ROOTFS_TARGETS "ROOTFS_TARGET_${ROOTFS_TARGET_ID}")
	math(EXPR ROOTFS_TARGET_ID "${ROOTFS_TARGET_ID}+1")
endmacro()

function (create_rootfs PROJNAME OUTDIR)
	# Define components
	set(REDISTRIBUTABLE_LIB_SOURCES )
	set(ROOTFS_MS_PLUGINS_LIB_SOURCES )
	if(WITH_DYNAMIC_MSVC_RUNTIME)
		get_filename_component(MSVCP_LIBNAME ${MSVCP_LIB} NAME)
		get_filename_component(MSVCR_LIBNAME ${MSVCR_LIB} NAME)
		list(APPEND REDISTRIBUTABLE_LIB_SOURCES ${MSVCP_LIBNAME} ${MSVCR_LIBNAME})
	endif()
	set(ROOTFS_LIB_SOURCES
		antlr3c.${DEPENDENCY_EXT}
		bellesip.${DEPENDENCY_EXT}
		linphone.${DEPENDENCY_EXT}
		mediastreamer_base.${DEPENDENCY_EXT}
		mediastreamer_voip.${DEPENDENCY_EXT}
		ortp.${DEPENDENCY_EXT}
		polarssl.${DEPENDENCY_EXT}
		speex.${DEPENDENCY_EXT}
		speexdsp.${DEPENDENCY_EXT}
	)
	if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		list(APPEND ROOTFS_LIB_SOURCES zlibd.${DEPENDENCY_EXT})
	else()
		list(APPEND ROOTFS_LIB_SOURCES zlib.${DEPENDENCY_EXT})
	endif()
	if(ENABLE_SRTP)
		list(APPEND ROOTFS_LIB_SOURCES srtp.${DEPENDENCY_EXT})
	endif()
	if(ENABLE_ZRTP)
		list(APPEND ROOTFS_LIB_SOURCES bzrtp.${DEPENDENCY_EXT})
	endif()
	if(ENABLE_FFMPEG)
		list(APPEND ROOTFS_LIB_SOURCES
			avcodec-53.${DEPENDENCY_EXT}
			avutil-51.${DEPENDENCY_EXT}
			swscale-2.${DEPENDENCY_EXT}
		)
	endif()
	if(ENABLE_TUNNEL)
		list(APPEND ROOTFS_LIB_SOURCES tunnel.${DEPENDENCY_EXT})
	endif()
	if(ENABLE_AMR)
		list(APPEND ROOTFS_MS_PLUGINS_LIB_SOURCES libmsamr.${DEPENDENCY_EXT})
	endif()
	if(ENABLE_G729)
		list(APPEND ROOTFS_MS_PLUGINS_LIB_SOURCES libmsbcg729.${DEPENDENCY_EXT})
	endif()
	if(ENABLE_ILBC)
		list(APPEND ROOTFS_MS_PLUGINS_LIB_SOURCES libmsilbc.${DEPENDENCY_EXT})
	endif()
	if(ENABLE_OPENH264)
		list(APPEND ROOTFS_MS_PLUGINS_LIB_SOURCES libmsopenh264.${DEPENDENCY_EXT})
	endif()
	list(APPEND ROOTFS_MS_PLUGINS_LIB_SOURCES libmswasapi.${DEPENDENCY_EXT})
	if(ENABLE_X264)
		list(APPEND ROOTFS_MS_PLUGINS_LIB_SOURCES libmsx264-0.${DEPENDENCY_EXT})
	endif()

	set(ROOTFS_SHARE_SOURCES
		linphone/rootca.pem
		images/nowebcamCIF.jpg
		sounds/linphone/ringback.wav
		sounds/linphone/rings/oldphone.wav
		sounds/linphone/rings/toy-mono.wav
	)

	# Install and sign libraries and files
	foreach(elem ${REDISTRIBUTABLE_LIB_SOURCES})
		add_rootfs_target(
			${PROJNAME}
			${FB_ROOTFS_DIR}
			"C:/Windows/System32"
			${elem}
			1
		)
	endforeach(elem)
	FOREACH(elem ${ROOTFS_LIB_SOURCES})
		add_rootfs_target(
			${PROJNAME}
			${FB_ROOTFS_DIR}
			${CMAKE_INSTALL_PREFIX}/bin
			${elem}
			1
		)
	ENDFOREACH(elem ${ROOTFS_LIB_SOURCES})
	FOREACH(elem ${ROOTFS_MS_PLUGINS_LIB_SOURCES})
		add_rootfs_target(
			${PROJNAME}
			${FB_ROOTFS_DIR}/${PLUGIN_SHAREDIR}/lib/mediastreamer/plugins
			${CMAKE_INSTALL_PREFIX}/lib/mediastreamer/plugins
			${elem}
			1
		)
	ENDFOREACH(elem ${ROOTFS_LIB_SOURCES})
	FOREACH(elem ${ROOTFS_SHARE_SOURCES})
		add_rootfs_target(
			${PROJNAME}
			${FB_ROOTFS_DIR}/${PLUGIN_SHAREDIR}/share
			${CMAKE_INSTALL_PREFIX}/share
			${elem}
			0
		)
	ENDFOREACH(elem ${ROOTFS_SHARE_SOURCES})

	add_custom_target(${PROJNAME}${FB_ROOTFS_SUFFIX} ALL
		DEPENDS ${ROOTFS_TARGETS}
		COMMAND ${CMAKE_COMMAND} -E copy ${OUTDIR}/${FBSTRING_PluginFileName}.pdb ${FB_ROOTFS_DIR}/ || ${CMAKE_COMMAND} -E echo "No pdb"
		COMMAND ${CMAKE_COMMAND} -E copy ${OUTDIR}/${FBSTRING_PluginFileName}.${PLUGIN_EXT} ${FB_ROOTFS_DIR}/
		COMMAND ${LW_SIGNTOOL_COMMAND} ${FB_ROOTFS_DIR}/${FBSTRING_PluginFileName}.${PLUGIN_EXT}
	)
	add_dependencies(${PROJNAME}${FB_ROOTFS_SUFFIX} ${PROJNAME})
	MESSAGE("-- Successfully added Rootfs creation step")
endfunction(create_rootfs)
###############################################################################

create_rootfs(${PLUGIN_NAME} ${FB_OUT_DIR})


###############################################################################
# MSI & EXE Package
if(LW_CREATE_MSI)
	SET(WIX_HEAT_FLAGS
		-gg                  # Generate GUIDs
		-srd                 # Suppress Root Dir
		-cg PluginDLLGroup   # Set the Component group name
		-dr INSTALLDIR       # Set the directory ID to put the files in
	)
	if (NOT FB_WIX_SUFFIX)
		set (FB_WIX_SUFFIX _MSI)
	endif()
	if (NOT FB_WIX_EXE_SUFFIX)
		set (FB_WIX_EXE_SUFFIX _EXE)
	endif()

	if(FB_ATLREG_MACHINEWIDE)
		set(WIX_FORCE_PER "machine")
		set(WIX_INSTALL_SCOPE "perMachine")
	else(FB_ATLREG_MACHINEWIDE)
		set(WIX_INSTALL_SCOPE "perUser")
	endif(FB_ATLREG_MACHINEWIDE)
	CONFIGURE_FILE_EXT(${CMAKE_CURRENT_SOURCE_DIR}/Win/WiX/linphoneInstaller.wxs ${CMAKE_CURRENT_BINARY_DIR}/Win/WiX/linphoneInstaller.wxs)
	SET(FB_WIX_DEST ${FB_OUT_DIR}/${PLUGIN_NAME}-${FBSTRING_PLUGIN_GIT_REVISION}-${FB_PACKAGE_SUFFIX}.msi)
	SET(FB_WIX_EXEDEST ${FB_OUT_DIR}/${PLUGIN_NAME}-${FBSTRING_PLUGIN_GIT_REVISION}-${FB_PACKAGE_SUFFIX}.exe)
	add_wix_installer(${PLUGIN_NAME}
		"${CMAKE_CURRENT_BINARY_DIR}/Win/WiX/linphoneInstaller.wxs"
		PluginDLLGroup
		${FB_ROOTFS_DIR}/
		"${FB_ROOTFS_DIR}/${FBSTRING_PluginFileName}.${PLUGIN_EXT}"
		${PLUGIN_NAME}${FB_ROOTFS_SUFFIX}
	)
	SET_TARGET_PROPERTIES(${PLUGIN_NAME}${FB_WIX_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
	SET_TARGET_PROPERTIES(${PLUGIN_NAME}${FB_WIX_EXE_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})

	my_sign_file(${PLUGIN_NAME}${FB_WIX_SUFFIX} "${FB_OUT_DIR}/${PLUGIN_NAME}-${FBSTRING_PLUGIN_GIT_REVISION}-${FB_PACKAGE_SUFFIX}.msi")
	my_sign_file(${PLUGIN_NAME}${FB_WIX_EXE_SUFFIX} "${FB_OUT_DIR}/${PLUGIN_NAME}-${FBSTRING_PLUGIN_GIT_REVISION}-${FB_PACKAGE_SUFFIX}.exe")
endif(LW_CREATE_MSI)
###############################################################################

###############################################################################
# CAB Package
if(LW_CREATE_CAB)
	SET(FB_CAB_DEST ${FB_OUT_DIR}/${PLUGIN_NAME}-${FBSTRING_PLUGIN_GIT_REVISION}-${FB_PACKAGE_SUFFIX}.cab)
	if (NOT FB_CAB_SUFFIX)
		set (FB_CAB_SUFFIX _CAB)
	endif()

	create_cab(${PLUGIN_NAME}
		"${CMAKE_CURRENT_SOURCE_DIR}/Win/Wix/linphone-web.ddf"
		"${CMAKE_CURRENT_SOURCE_DIR}/Win/Wix/linphone-web.inf"
		${FB_OUT_DIR}/
		${PLUGIN_NAME}${FB_WIX_EXE_SUFFIX}
	)
	SET_TARGET_PROPERTIES(${PLUGIN_NAME}${FB_CAB_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})

	my_sign_file(${PLUGIN_NAME}${FB_CAB_SUFFIX} "${FB_OUT_DIR}/${PLUGIN_NAME}-${FBSTRING_PLUGIN_GIT_REVISION}-${FB_PACKAGE_SUFFIX}.cab")
endif(LW_CREATE_CAB)
###############################################################################

###############################################################################
# XPI Package
if(LW_CREATE_XPI)
	if (NOT FB_XPI_PACKAGE_SUFFIX)
		SET(FB_XPI_PACKAGE_SUFFIX _XPI)
	endif()

	function (create_xpi_package PROJNAME PROJVERSION OUTDIR PROJDEP)
		SET(XPI_SOURCES
			${CMAKE_CURRENT_BINARY_DIR}/install.rdf
			${CMAKE_CURRENT_SOURCE_DIR}/Win/XPI/bootstrap.js
			${CMAKE_CURRENT_BINARY_DIR}/chrome.manifest
			${CMAKE_CURRENT_SOURCE_DIR}/Common/icon48.png
			${CMAKE_CURRENT_SOURCE_DIR}/Common/icon64.png
		)

		CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Win/XPI/install.rdf ${CMAKE_CURRENT_BINARY_DIR}/install.rdf)
		CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Win/XPI/chrome.manifest ${CMAKE_CURRENT_BINARY_DIR}/chrome.manifest)

		SET(FB_PKG_DIR ${FB_OUT_DIR}/XPI)

		ADD_CUSTOM_TARGET(${PROJNAME}${FB_XPI_PACKAGE_SUFFIX} ALL
					DEPENDS ${XPI_SOURCES}
					COMMAND ${CMAKE_COMMAND} -E remove_directory ${FB_PKG_DIR}
					COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}
					COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/install.rdf ${FB_PKG_DIR}/
					COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Win/XPI/bootstrap.js ${FB_PKG_DIR}/
					COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/chrome.manifest ${FB_PKG_DIR}/

					COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/chrome/skin
					COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Common/icon48.png ${FB_PKG_DIR}/chrome/skin/
					COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Common/icon64.png ${FB_PKG_DIR}/chrome/skin/

					COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Common/copy.py ${FB_ROOTFS_DIR} ${FB_PKG_DIR}/plugins

					COMMAND ${CMAKE_COMMAND} -E remove ${OUTDIR}/${PROJNAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.xpi
					COMMAND ${Java_JAR_EXECUTABLE} cfM ${OUTDIR}/${PROJNAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.xpi -C ${FB_PKG_DIR} .

					COMMAND ${CMAKE_COMMAND} -E touch ${FB_OUT_DIR}/XPI.updated
		)
		SET_TARGET_PROPERTIES(${PROJNAME}${FB_XPI_PACKAGE_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
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
endif(LW_CREATE_XPI)
###############################################################################

###############################################################################
# CRX Package
if(LW_CREATE_CRX)
	if (NOT FB_CRX_PACKAGE_SUFFIX)
		SET(FB_CRX_PACKAGE_SUFFIX _CRX)
	endif()

	function (create_crx_package PROJNAME PROJVERSION OUTDIR PROJDEP)
		SET(CRX_SOURCES
			${CMAKE_CURRENT_BINARY_DIR}/manifest.json
			${CMAKE_CURRENT_SOURCE_DIR}/Common/icon16.png
			${CMAKE_CURRENT_SOURCE_DIR}/Common/icon48.png
		)

		CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Win/CRX/manifest.json ${CMAKE_CURRENT_BINARY_DIR}/manifest.json)

		SET(FB_PKG_DIR ${FB_OUT_DIR}/CRX)

		ADD_CUSTOM_TARGET(${PROJNAME}${FB_CRX_PACKAGE_SUFFIX}
					DEPENDS ${CRX_SOURCES}
					COMMAND ${CMAKE_COMMAND} -E remove_directory ${FB_PKG_DIR}
					COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Common/copy.py ${FB_ROOTFS_DIR} ${FB_PKG_DIR}
					COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/manifest.json ${FB_PKG_DIR}/
					COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Common/icon16.png ${FB_PKG_DIR}/
					COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Common/icon48.png ${FB_PKG_DIR}/

					COMMAND ${CMAKE_COMMAND} -E remove ${OUTDIR}/${PROJECT_NAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.crx
					COMMAND ${Java_JAR_EXECUTABLE} cfM ${OUTDIR}/${PROJECT_NAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-unsigned.crx -C ${FB_PKG_DIR} .

					COMMAND ${CMAKE_COMMAND} -E touch ${FB_OUT_DIR}/CRX.updated
		)
		SET_TARGET_PROPERTIES(${PROJNAME}${FB_CRX_PACKAGE_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
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
endif(LW_CREATE_CRX)
###############################################################################
