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
	-DPLUGIN_SHAREDIR="${PLUGIN_SHAREDIR}"
	-std=c++0x
)

INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIRS})
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

SET(FB_PACKAGE_SUFFIX Linux)
IF(FB_PLATFORM_ARCH_64)
	SET(FB_PACKAGE_SUFFIX "${FB_PACKAGE_SUFFIX}-x86_64")
ELSE(FB_PLATFORM_ARCH_64)
	SET(FB_PACKAGE_SUFFIX "${FB_PACKAGE_SUFFIX}-x86")
ENDIF(FB_PLATFORM_ARCH_64)

SET(DEPENDENCY_EXT "so")
SET(PLUGIN_EXT "so")
SET(FB_OUT_DIR ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR})
SET(FB_ROOTFS_DIR ${FB_OUT_DIR}/Rootfs)

# Use default chrpath if not defined
IF(NOT DEFINED CMAKE_CHRPATH)
	SET(CMAKE_CHRPATH chrpath)
ENDIF(NOT DEFINED CMAKE_CHRPATH)

# Use default chmod if not defined
IF(NOT DEFINED CMAKE_CHMOD)
	SET(CMAKE_CHMOD chmod)
ENDIF(NOT DEFINED CMAKE_CHMOD)

###############################################################################
# Create Rootfs
if (NOT FB_ROOTFS_SUFFIX)
	SET(FB_ROOTFS_SUFFIX _RootFS)
endif()

function (create_rootfs PROJNAME)
	# Define components
	SET(ROOTFS_LIB_SOURCES
		libjpeg.${DEPENDENCY_EXT}.8
		liblinphone.${DEPENDENCY_EXT}.5
		libmediastreamer_base.${DEPENDENCY_EXT}.3
		libmediastreamer_voip.${DEPENDENCY_EXT}.3
		libogg.${DEPENDENCY_EXT}.0
		libopus.${DEPENDENCY_EXT}.0.4.0
		libortp.${DEPENDENCY_EXT}.9
		libspeex.${DEPENDENCY_EXT}.1
		libspeexdsp.${DEPENDENCY_EXT}.1
		libtheora.${DEPENDENCY_EXT}.0
		libv4l1.${DEPENDENCY_EXT}.0
		libv4l2.${DEPENDENCY_EXT}.0
		libv4lconvert.${DEPENDENCY_EXT}.0
		libvpx.${DEPENDENCY_EXT}.1
		libz.${DEPENDENCY_EXT}.1
	)
	IF(LW_USE_SRTP)
		SET(ROOTFS_LIB_SOURCES
			${ROOTFS_LIB_SOURCES}
			libsrtp.${DEPENDENCY_EXT}.1.4.5
		)
	ENDIF(LW_USE_SRTP)
	IF(LW_USE_OPENSSL)
		SET(ROOTFS_LIB_SOURCES
			${ROOTFS_LIB_SOURCES}
			libcrypto.${DEPENDENCY_EXT}.1.0.0
			libssl.${DEPENDENCY_EXT}.1.0.0
		)
	ENDIF(LW_USE_OPENSSL)
	IF(LW_USE_EXOSIP)
		SET(ROOTFS_LIB_SOURCES
			${ROOTFS_LIB_SOURCES}
			libeXosip2.${DEPENDENCY_EXT}.7
			libosip2.${DEPENDENCY_EXT}.7
			libosipparser2.${DEPENDENCY_EXT}.7
		)
	ENDIF(LW_USE_EXOSIP)
	IF(LW_USE_FFMPEG)
		SET(ROOTFS_LIB_SOURCES
			${ROOTFS_LIB_SOURCES}
			libavcodec.${DEPENDENCY_EXT}.53
			libavutil.${DEPENDENCY_EXT}.51
			libswscale.${DEPENDENCY_EXT}.2
		)
	ENDIF(LW_USE_FFMPEG)
	IF(LW_USE_POLARSSL)
		SET(ROOTFS_LIB_SOURCES
			${ROOTFS_LIB_SOURCES}
			libpolarssl.${DEPENDENCY_EXT}.0
		)
	ENDIF(LW_USE_POLARSSL)
	IF(LW_USE_BELLESIP)
		SET(ROOTFS_LIB_SOURCES
			${ROOTFS_LIB_SOURCES}
			libbellesip.${DEPENDENCY_EXT}.0
			libantlr3c.${DEPENDENCY_EXT}.0
			libxml2.${DEPENDENCY_EXT}.2
		)
	ENDIF(LW_USE_BELLESIP)

	SET(ROOTFS_SHARE_SOURCES
		linphone/rootca.pem
		images/nowebcamCIF.jpg
		sounds/linphone/ringback.wav
		sounds/linphone/rings/oldphone.wav
		sounds/linphone/rings/toy-mono.wav
	)

	# Set Rootfs sources
	SET(ROOTFS_SOURCES
		${FB_OUT_DIR}/${FBSTRING_PluginFileName}.${PLUGIN_EXT}
	)
	FOREACH(elem ${ROOTFS_LIB_SOURCES})
		SET(DIR_SRC ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib)
		SET(DIR_DEST ${FB_ROOTFS_DIR}/${PLUGIN_SHAREDIR})
		GET_FILENAME_COMPONENT(path ${elem} PATH)
		ADD_CUSTOM_COMMAND(OUTPUT ${DIR_DEST}/${elem} 
			DEPENDS ${DIR_SRC}/${elem}
			COMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_DEST}/${path}
			COMMAND ${CMAKE_COMMAND} -E copy ${DIR_SRC}/${elem} ${DIR_DEST}/${elem}
			COMMAND ${CMAKE_CHMOD} +w ${DIR_DEST}/${elem}
			COMMAND ${CMAKE_CHRPATH} -c -r \\\$$ORIGIN ${DIR_DEST}/${elem} || echo "Error ignored"
			COMMAND ${CMAKE_CHMOD} -w ${DIR_DEST}/${elem}
		)
		LIST(APPEND ROOTFS_SOURCES ${DIR_DEST}/${elem})
	ENDFOREACH(elem ${ROOTFS_LIB_SOURCES})
	FOREACH(elem ${ROOTFS_SHARE_SOURCES})
		SET(DIR_SRC ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share)
		SET(DIR_DEST ${FB_ROOTFS_DIR}/${PLUGIN_SHAREDIR}/share)
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
		COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${FBSTRING_PluginFileName}.${PLUGIN_EXT} ${FB_ROOTFS_DIR}/

		COMMAND ${CMAKE_COMMAND} -E touch ${FB_OUT_DIR}/Rootfs.updated
	)
	
	ADD_CUSTOM_TARGET(${PROJNAME}${FB_ROOTFS_SUFFIX} ALL DEPENDS ${FB_OUT_DIR}/Rootfs.updated)
	SET_TARGET_PROPERTIES(${PROJNAME}${FB_ROOTFS_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
	ADD_DEPENDENCIES(${PROJNAME}${FB_ROOTFS_SUFFIX} ${PROJNAME})
	MESSAGE("-- Successfully added Rootfs step")
endfunction(create_rootfs)
###############################################################################

create_rootfs(${PLUGIN_NAME})

# Add rpath to generated library
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,'\$ORIGIN/${PLUGIN_SHAREDIR}'" )

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

	ADD_CUSTOM_TARGET(${PROJNAME}${FB_TGZ_PACKAGE_SUFFIX} ALL DEPENDS ${OUTDIR}/${PROJECT_NAME}-${FBSTRING_PLUGIN_VERSION}-${FB_PACKAGE_SUFFIX}.tar.gz)
	SET_TARGET_PROPERTIES(${PROJNAME}${FB_TGZ_PACKAGE_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
	ADD_CUSTOM_COMMAND(OUTPUT ${OUTDIR}/${PROJECT_NAME}-${FBSTRING_PLUGIN_VERSION}-${FB_PACKAGE_SUFFIX}.tar.gz
				DEPENDS ${TGZ_SOURCES}
				COMMAND ${CMAKE_COMMAND} -E remove_directory ${FB_PKG_DIR}
				COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Common/copy.py ${FB_ROOTFS_DIR} ${FB_PKG_DIR}

				COMMAND ${CMAKE_COMMAND} -E remove ${OUTDIR}/${PROJECT_NAME}-${FBSTRING_PLUGIN_VERSION}-${FB_PACKAGE_SUFFIX}.tar.gz
				COMMAND tar zcvf ${OUTDIR}/${PROJECT_NAME}-${FBSTRING_PLUGIN_VERSION}-${FB_PACKAGE_SUFFIX}.tar.gz -C ${FB_PKG_DIR} .

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
###############################################################################


###############################################################################
# Documentation Package
if (NOT FB_DOC_PACKAGE_SUFFIX)
	SET(FB_DOC_PACKAGE_SUFFIX _DOC)
endif()

function (create_documentation_package PROJNAME PROJVERSION OUTDIR PROJDEP)
	SET(DOC_SOURCES
		${FB_OUT_DIR}/Rootfs.updated
		${CMAKE_CURRENT_SOURCE_DIR}/README.md
		${DOCUMENTATION}
	)

	SET(FB_PKG_DIR ${FB_OUT_DIR}/Doc)
	SET(JSWRAPPER_DIR ${FB_PKG_DIR}/jswrapper)

	ADD_CUSTOM_TARGET(${PROJNAME}${FB_DOC_PACKAGE_SUFFIX} ALL DEPENDS ${OUTDIR}/${PROJECT_NAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-doc.zip)
	SET_TARGET_PROPERTIES(${PROJNAME}${FB_DOC_PACKAGE_SUFFIX} PROPERTIES FOLDER ${FBSTRING_ProductName})
	ADD_CUSTOM_COMMAND(OUTPUT ${OUTDIR}/${PROJECT_NAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-doc.zip
				DEPENDS ${DOC_SOURCES}
				COMMAND ${CMAKE_COMMAND} -E remove_directory ${FB_PKG_DIR}
				COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}
				COMMAND ${CMAKE_COMMAND} -E remove_directory ${JSWRAPPER_DIR}
				COMMAND ${CMAKE_COMMAND} -E make_directory ${JSWRAPPER_DIR}
				COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/README.md ${FB_PKG_DIR}
				COMMAND cd ${JSWRAPPER_DIR} && lp-gen-wrappers --output javascript --project linphone ${DOCUMENTATION}
				COMMAND cd ${FB_PKG_DIR} && jsdoc --recurse --destination ${PROJECT_NAME}-${PROJVERSION} ${JSWRAPPER_DIR} ${FB_PKG_DIR}/README.md
				COMMAND cd ${FB_PKG_DIR} && zip -r ${OUTDIR}/${PROJECT_NAME}-${PROJVERSION}-${FB_PACKAGE_SUFFIX}-doc.zip ${PROJECT_NAME}-${PROJVERSION}
	)
	ADD_DEPENDENCIES(${PROJNAME}${FB_DOC_PACKAGE_SUFFIX} ${PROJDEP})
	MESSAGE("-- Successfully added documentation package step")
endfunction(create_documentation_package)

create_documentation_package(${PLUGIN_NAME}
	${FBSTRING_PLUGIN_VERSION}
	${FB_OUT_DIR}
	${PLUGIN_NAME}${FB_ROOTFS_SUFFIX}
)
###############################################################################
