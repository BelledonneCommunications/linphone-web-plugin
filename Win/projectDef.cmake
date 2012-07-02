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

# Windows template platform definition CMake file
# Included from ../CMakeLists.txt
include(${CMAKE_CURRENT_SOURCE_DIR}/Common/common.cmake)

# remember that the current source dir is the project root; this file is in Win/
file (GLOB PLATFORM RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Win/[^.]*.cpp
    Win/[^.]*.h
    Win/[^.]*.cmake
    )

# use this to add preprocessor definitions
add_definitions(
    /D "_ATL_STATIC_REGISTRY"
)

SOURCE_GROUP(Win FILES ${PLATFORM})

set (SOURCES
    ${SOURCES}
    ${PLATFORM}
    )
	
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE
        "/LTCG /SUBSYSTEM:WINDOWS /OPT:NOREF /OPT:ICF")
set(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL
        "/LTCG /SUBSYSTEM:WINDOWS /OPT:NOREF /OPT:ICF")
set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO
        "/LTCG /SUBSYSTEM:WINDOWS /OPT:NOREF /OPT:ICF")
include_directories(Rootfs/include/linphone)
include_directories(Rootfs/include)

add_windows_plugin(${PROJECT_NAME} SOURCES)

# This is an example of how to add a build step to sign the plugin DLL before
# the WiX installer builds.  The first filename (certificate.pfx) should be
# the path to your pfx file.  If it requires a passphrase, the passphrase
# should be located inside the second file. If you don't need a passphrase
# then set the second filename to "".  If you don't want signtool to timestamp
# your DLL then make the last parameter "".
#
# Note that this will not attempt to sign if the certificate isn't there --
# that's so that you can have development machines without the cert and it'll
# still work. Your cert should only be on the build machine and shouldn't be in
# source control!
# -- uncomment lines below this to enable signing --
firebreath_sign_plugin(${PROJECT_NAME}
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")

# add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!
target_link_libraries(${PROJECT_NAME} 
	${PLUGIN_INTERNAL_DEPS}
	"${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/liblinphone-5.lib"
	"${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libmediastreamer-1.lib"
	"${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libortp-8.lib"
	)

SET (LINPHONEWEB_SHAREDIR linphoneweb)
SET (FB_OUT_DIR ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR})

#Copy dll dependencies
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} 
		  PRE_BUILD
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/avcodec-53.dll ${FB_OUT_DIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/avutil-51.dll ${FB_OUT_DIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libeXosip2-7.dll ${FB_OUT_DIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libeay32.dll ${FB_OUT_DIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/liblinphone-5.dll ${FB_OUT_DIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libmediastreamer-1.dll ${FB_OUT_DIR}/		  
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libogg-0.dll ${FB_OUT_DIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libortp-8.dll ${FB_OUT_DIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libosip2-7.dll ${FB_OUT_DIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libosipparser2-7.dll ${FB_OUT_DIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libspeex-1.dll ${FB_OUT_DIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libspeexdsp-1.dll ${FB_OUT_DIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libtheora-0.dll ${FB_OUT_DIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libvpx-1.dll ${FB_OUT_DIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libz-1.dll ${FB_OUT_DIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/ssleay32.dll ${FB_OUT_DIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/swscale-2.dll ${FB_OUT_DIR}/
		  
		  COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/share/
		  COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/
		  COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/
		  COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share/sounds/linphone/ringback.wav ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/share/sounds/linphone/rings/oldphone.wav ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/
)

# Sign dll dependencies
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_OUT_DIR}/avcodec-53.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_OUT_DIR}/avutil-51.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_OUT_DIR}/libeXosip2-7.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_OUT_DIR}/libeay32.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_OUT_DIR}/liblinphone-5.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pfx" 
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_OUT_DIR}/libmediastreamer-1.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_OUT_DIR}/libogg-0.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_OUT_DIR}/libortp-8.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_OUT_DIR}/libosip2-7.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_OUT_DIR}/libosipparser2-7.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_OUT_DIR}/libspeex-1.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_OUT_DIR}/libspeexdsp-1.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_OUT_DIR}/libtheora-0.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_OUT_DIR}/libvpx-1.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_OUT_DIR}/libz-1.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_OUT_DIR}/ssleay32.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_OUT_DIR}/swscale-2.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
	
SET(WIX_HEAT_FLAGS
    -gg                 # Generate GUIDs
    -srd                # Suppress Root Dir
    -cg PluginDLLGroup  # Set the Component group name
    -dr INSTALLDIR      # Set the directory ID to put the files in
    )

SET(FB_WIX_DEST ${FB_OUT_DIR}/${PLUGIN_NAME}-${FBSTRING_PLUGIN_VERSION}.msi)
add_wix_installer(${PLUGIN_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/Win/WiX/linphoneInstaller.wxs
    PluginDLLGroup
    ${FB_OUT_DIR}/
    ${FB_OUT_DIR}/${FBSTRING_PluginFileName}.dll
    ${PROJECT_NAME}
    )

# This is an example of how to add a build step to sign the WiX installer
# -- uncomment lines below this to enable signing --
firebreath_sign_file(${PLUGIN_NAME}_WiXInstall
    "${FB_OUT_DIR}/${PLUGIN_NAME}-${FBSTRING_PLUGIN_VERSION}.msi"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
	
IF(NOT DEFINED CMAKE_MAKECAB)
	SET(CMAKE_MAKECAB makecab)
ENDIF(NOT DEFINED CMAKE_MAKECAB)

function (create_cab PROJNAME DDF FILES_CAB)
    GET_FILENAME_COMPONENT(_tmp_File ${DDF} NAME)
    configure_file(${DDF} ${CMAKE_CURRENT_BINARY_DIR}/${_tmp_File})
    message("Configuring ${DDF} -> ${CMAKE_CURRENT_BINARY_DIR}/${_tmp_File}")
    set(DDF ${CMAKE_CURRENT_BINARY_DIR}/${_tmp_File})

    FOREACH(_curFile ${FILES_CAB})
        GET_FILENAME_COMPONENT(_tmp_File ${_curFile} NAME)
        configure_file(${_curFile} ${CMAKE_CURRENT_BINARY_DIR}/${_tmp_File})
        message("Configuring ${_curFile} -> ${CMAKE_CURRENT_BINARY_DIR}/${_tmp_File}")
    ENDFOREACH()
	
	ADD_CUSTOM_TARGET(${PROJNAME}_exe DEPENDS ${PROJNAME}_WiXInstall)
	SET(WIX_SETUPBLD ${WIX_ROOT_DIR}/bin/setupbld.exe)
	ADD_CUSTOM_COMMAND(
		TARGET ${PROJECT_NAME}_exe
        COMMAND ${WIX_SETUPBLD} 
		ARGS -out "${FB_OUT_DIR}/${PLUGIN_NAME}-${FBSTRING_PLUGIN_VERSION}.exe" -mpsu "${FB_OUT_DIR}/${PLUGIN_NAME}-${FBSTRING_PLUGIN_VERSION}.msi" -setup ${WIX_ROOT_DIR}/bin/setup.exe
	)
	message("-- Successfully added EXE step")
	
	
	ADD_CUSTOM_TARGET(${PROJNAME}_cab DEPENDS ${PROJNAME}_exe)
	ADD_CUSTOM_COMMAND(
		TARGET ${PROJNAME}_cab
        COMMAND ${CMAKE_MAKECAB} 
		ARGS /D "BINSRC=${FB_OUT_DIR}/" /F "${DDF}"
	)
	message("-- Successfully added CAB step")
endfunction(create_cab)

create_cab(${PLUGIN_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/Win/Wix/linphone-web.ddf" "${CMAKE_CURRENT_SOURCE_DIR}/Win/Wix/linphone-web.inf")

firebreath_sign_file(${PLUGIN_NAME}_exe
    "${FB_OUT_DIR}/${PLUGIN_NAME}-${FBSTRING_PLUGIN_VERSION}.exe"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")

firebreath_sign_file(${PLUGIN_NAME}_cab
    "${FB_OUT_DIR}/${PLUGIN_NAME}-${FBSTRING_PLUGIN_VERSION}.cab"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")

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
	
	set(FB_PKG_DIR ${FB_OUT_DIR}/XPI)
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
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${ONAME}.dll ${FB_PKG_DIR}/plugins/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/avcodec-53.dll ${FB_PKG_DIR}/plugins/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/avutil-51.dll ${FB_PKG_DIR}/plugins/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/libeXosip2-7.dll ${FB_PKG_DIR}/plugins/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/libeay32.dll ${FB_PKG_DIR}/plugins/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/liblinphone-5.dll ${FB_PKG_DIR}/plugins/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/libmediastreamer-1.dll ${FB_PKG_DIR}/plugins/		  
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/libogg-0.dll ${FB_PKG_DIR}/plugins/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/libortp-8.dll ${FB_PKG_DIR}/plugins/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/libosip2-7.dll ${FB_PKG_DIR}/plugins/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/libosipparser2-7.dll ${FB_PKG_DIR}/plugins/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/libspeex-1.dll ${FB_PKG_DIR}/plugins/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/libspeexdsp-1.dll ${FB_PKG_DIR}/plugins/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/libtheora-0.dll ${FB_PKG_DIR}/plugins/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/libvpx-1.dll ${FB_PKG_DIR}/plugins/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/libz-1.dll ${FB_PKG_DIR}/plugins/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/ssleay32.dll ${FB_PKG_DIR}/plugins/
		         COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/swscale-2.dll ${FB_PKG_DIR}/plugins/
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/share/
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/share/sounds/
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/
                 COMMAND ${CMAKE_COMMAND} -E make_directory ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/ringback.wav ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/
                 COMMAND ${CMAKE_COMMAND} -E copy ${FB_OUT_DIR}/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/oldphone.wav ${FB_PKG_DIR}/plugins/${LINPHONEWEB_SHAREDIR}/share/sounds/linphone/rings/
                 COMMAND jar cfM ${OUTDIR}/${PROJECT_NAME}-${PROJVERSION}-win32-unsigned.xpi
                 		-C ${FB_PKG_DIR} .
	)
	ADD_DEPENDENCIES(${PROJNAME}${FB_XPI_PACKAGE_SUFFIX} ${PROJNAME})
	message("-- Successfully added XPI package step")
endfunction(create_xpi_package)
###############################################################################

create_xpi_package(${PLUGIN_NAME} ${FBSTRING_PLUGIN_VERSION} ${FB_OUT_DIR})

create_signed_xpi(${PLUGIN_NAME} 
	"${FB_OUT_DIR}/${PROJECT_NAME}-${FBSTRING_PLUGIN_VERSION}-win32-unsigned.xpi"
	"${FB_OUT_DIR}/${PROJECT_NAME}-${FBSTRING_PLUGIN_VERSION}-win32.xpi"
	"${CMAKE_CURRENT_SOURCE_DIR}/sign/linphoneweb.pem"
	"${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
	${PLUGIN_NAME}_PKG_XPI
)
