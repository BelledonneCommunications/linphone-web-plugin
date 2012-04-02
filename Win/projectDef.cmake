#/**********************************************************\ 
# Auto-generated Windows project definition file for the
# linphone project
#\**********************************************************/

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
include_directories(Libs/include/linphone)
include_directories(Libs/include)

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
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/belledonne-communications.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")

# add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!
target_link_libraries(${PROJECT_NAME} 
	${PLUGIN_INTERNAL_DEPS}
	"${CMAKE_CURRENT_SOURCE_DIR}/Libs/lib/liblinphone.lib"
	"${CMAKE_CURRENT_SOURCE_DIR}/Libs/lib/libmediastreamer.lib"
	)

#Copy dll dependencies
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} 
		  PRE_BUILD
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Libs/bin/avcodec-53.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Libs/bin/avutil-51.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Libs/bin/libeXosip2-7.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Libs/bin/libintl-8.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Libs/bin/liblinphone-4.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Libs/bin/libmediastreamer-1.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/		  
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Libs/bin/libogg-0.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Libs/bin/libortp-8.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Libs/bin/libosip2-7.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Libs/bin/libosipparser2-7.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Libs/bin/libspeex-1.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Libs/bin/libspeexdsp-1.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Libs/bin/libtheora-0.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Libs/bin/libvorbis-0.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Libs/bin/swscale-2.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
		  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Libs/bin/zlib1.dll ${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/
)

# Sign dll dependencies
firebreath_sign_file("${PLUGIN_NAME}"
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/avcodec-53.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/belledonne-communications.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/avutil-51.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/belledonne-communications.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libeXosip2-7.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/belledonne-communications.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libintl-8.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/belledonne-communications.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/liblinphone-4.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/belledonne-communications.pfx" 
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libmediastreamer-1.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/belledonne-communications.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libogg-0.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/belledonne-communications.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libortp-8.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/belledonne-communications.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libosip2-7.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/belledonne-communications.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libosipparser2-7.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/belledonne-communications.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libspeex-1.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/belledonne-communications.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libspeexdsp-1.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/belledonne-communications.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libtheora-0.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/belledonne-communications.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/libvorbis-0.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/belledonne-communications.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/swscale-2.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/belledonne-communications.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")
firebreath_sign_file(${PLUGIN_NAME}
    "${FB_BIN_DIR}/${PLUGIN_NAME}/${CMAKE_CFG_INTDIR}/zlib1.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/belledonne-communications.pfx"
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
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/belledonne-communications.pfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/sign/passphrase.txt"
    "http://timestamp.verisign.com/scripts/timestamp.dll")