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
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphonejs.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")

# add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!
target_link_libraries(${PROJECT_NAME} 
	${PLUGIN_INTERNAL_DEPS}
	"${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/liblinphone-5.lib"
	"${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/lib/libmediastreamer-1.lib"
	)

#Copy dll dependencies
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} 
		  PRE_BUILD
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/avcodec-53.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/avutil-51.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libeXosip2-7.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libeay32.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/liblinphone-5.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libmediastreamer-1.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/		  
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libogg-0.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libortp-8.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libosip2-7.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libosipparser2-7.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libspeex-1.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libspeexdsp-1.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libtheora-0.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/libz-1.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/ssleay32.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Rootfs/bin/swscale-2.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
)

# Sign dll dependencies
firebreath_sign_file("${PLUGIN_NAME}"
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/avcodec-53.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphonejs.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/avutil-51.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphonejs.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libeXosip2-7.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphonejs.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libeay32.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphonejs.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/liblinphone-5.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphonejs.pfx" 
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libmediastreamer-1.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphonejs.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libogg-0.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphonejs.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libortp-8.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphonejs.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libosip2-7.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphonejs.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libosipparser2-7.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphonejs.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libspeex-1.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphonejs.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libspeexdsp-1.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphonejs.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libtheora-0.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphonejs.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libz-1.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphonejs.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/ssleay32.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphonejs.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/swscale-2.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphonejs.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
	
set(WIX_HEAT_FLAGS
    -gg                 # Generate GUIDs
    -srd                # Suppress Root Dir
    -cg PluginDLLGroup  # Set the Component group name
    -dr INSTALLDIR      # Set the directory ID to put the files in
    )

add_wix_installer( ${PLUGIN_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/Win/WiX/linphoneInstaller.wxs
    PluginDLLGroup
    ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
    ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/${FBSTRING_PluginFileName}.dll
    ${PROJECT_NAME}
    )

# This is an example of how to add a build step to sign the WiX installer
# -- uncomment lines below this to enable signing --
firebreath_sign_file(${PLUGIN_NAME}_WiXInstall
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/${PLUGIN_NAME}.msi"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphonejs.pfx"
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
		ARGS -out "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/${PLUGIN_NAME}.exe" -mpsu "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/${PLUGIN_NAME}.msi" -setup ${WIX_ROOT_DIR}/bin/setup.exe
	)
	message("-- Successfully added EXE step")
	
	
	ADD_CUSTOM_TARGET(${PROJNAME}_cab DEPENDS ${PROJNAME}_exe)
	ADD_CUSTOM_COMMAND(
		TARGET ${PROJNAME}_cab
        COMMAND ${CMAKE_MAKECAB} 
		ARGS /D "BINSRC=${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/" /F "${DDF}"
	)
	message("-- Successfully added CAB step")
endfunction(create_cab)

create_cab(${PLUGIN_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/Win/Wix/linphone-web.ddf" "${CMAKE_CURRENT_SOURCE_DIR}/Win/Wix/linphone-web.inf")

firebreath_sign_file(${PLUGIN_NAME}_exe
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/${PLUGIN_NAME}.exe"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphonejs.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")

firebreath_sign_file(${PLUGIN_NAME}_cab
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/${PLUGIN_NAME}.cab"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/linphonejs.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
