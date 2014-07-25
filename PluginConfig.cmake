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

find_package(Git REQUIRED)

# Project build properties
set(PLUGIN_NAME "linphone-web")
set(PLUGIN_PREFIX "LINWEB")
set(COMPANY_NAME "belledonne-communications")
set(PLUGIN_SHAREDIR "linphoneweb")
set(MAC_CODE_SIGNING_IDENTITY "Developer ID Application: Belledonne communications")
set(MAC_INSTALLER_SIGNING_IDENTITY "Developer ID Installer: Belledonne communications")

# ActiveX constants:
set(FBTYPELIB_NAME "LinphoneWeb")
set(FBTYPELIB_DESC "Linphone Web 1.0 Type Library")
set(IFBControl_DESC "Linphone Web Control Interface")
set(FBControl_DESC "Linphone Web Control Class")
set(IFBComJavascriptObject_DESC "Linphone Web IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "Linphone Web ComJavascriptObject Class")
set(IFBComEventSource_DESC "Linphone Web IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID 65fd31b0-c3b6-5741-b010-c033e6766a1d)
set(IFBControl_GUID 34538382-721c-5b50-be7f-a2b384eb36c6)
set(IFBComJavascriptObject_GUID fa963671-1cb5-537b-baa8-d3b60c13e09c)
set(FBComJavascriptObject_GUID b4228fbd-ed98-5619-a71b-93b3e4fe422d)
set(IFBComEventSource_GUID 898a611e-b63b-58c7-bc64-0c0fa0c0cded)

# these are the pieces that are relevant to using it from Javascript
set(MOZILLA_PLUGINID "belledonne-communications.com/linphone/web")

# Company name
set(FBSTRING_CompanyName "Belledonne Communications")

# Company id
set(FBSTRING_CompanyDomain "com.belledonne-communications")

execute_process(WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
	COMMAND ${GIT_EXECUTABLE} "describe" "--always"
	OUTPUT_VARIABLE FBSTRING_PLUGIN_GIT_REVISION
	OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REGEX REPLACE "^([0-9.]+)-.*$" "\\1" FBSTRING_PLUGIN_VERSION ${FBSTRING_PLUGIN_GIT_REVISION})
string(REPLACE "." "," FBSTRING_PLUGIN_VERSION_ACTIVEX ${FBSTRING_PLUGIN_VERSION})
set(FBSTRING_LegalCopyright "Copyright 2013 Belledonne Communications")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}-${FBSTRING_PLUGIN_VERSION}")

# Internal settings name (common between all version of plugin)
set(FBSTRING_ProductName "Linphone Web")
# Internal settings domain (common between all version of plugin)
set(FBSTRING_ProductDomain "linphone-web")

# Shown name
set(FBSTRING_PluginName "Linphone Web")
# Plugin id
set(FBSTRING_PluginDomain "linphone-web")

set(FBSTRING_FileDescription "Web plugin of Linphone")

set(FBSTRING_FileExtents "")
set(FBSTRING_MIMEType_Core "application/x-linphone-web")
set(FBSTRING_MIMEType_Video "application/x-linphone-web-video")
set(FBSTRING_MIMEType 
	${FBSTRING_MIMEType_Core}
	${FBSTRING_MIMEType_Video})
set(ACTIVEX_PROGID 
	"com.belledonne-communications.linphone.web"
	"com.belledonne-communications.linphone.web.video")
set(FBControl_GUID 
	1563fddd-83a0-553a-854b-a6f75ed2d5f6
	1563fddd-83a0-553a-854b-a6f75ed2d5f7)
set(FBSTRING_PluginDescription 
	"Linphone Web Plugin"
	"Linphone Web Video Plugin")

# Version variable
set(FBSTRING_V_TXT_WIN "Windows")
set(FBSTRING_V_TXT_MAC "Mac")
set(FBSTRING_V_TXT_X11 "Linux")

set(FBSTRING_V_TXT_X86 "")
set(FBSTRING_V_X86 "")
set(FBSTRING_V_TXT_X86_64 "64 bits")
set(FBSTRING_V_X86_64 "64")

# Compose using os and arch
if(FB_PLATFORM_NAME STREQUAL "Win")
	if(NOT ${FBSTRING_V_TXT_WIN} STREQUAL "")
		set(FBSTRING_PluginName "${FBSTRING_PluginName} ${FBSTRING_V_TXT_WIN}")
	endif(NOT ${FBSTRING_V_TXT_WIN} STREQUAL "")
endif(FB_PLATFORM_NAME STREQUAL "Win") 
if(FB_PLATFORM_NAME STREQUAL "Mac")
	if(NOT ${FBSTRING_V_TXT_MAC} STREQUAL "")
		set(FBSTRING_PluginName "${FBSTRING_PluginName} ${FBSTRING_V_TXT_MAC}")
	endif(NOT ${FBSTRING_V_TXT_MAC} STREQUAL "")
endif(FB_PLATFORM_NAME STREQUAL "Mac") 
if(FB_PLATFORM_NAME STREQUAL "X11")
	if(NOT ${FBSTRING_V_TXT_X11} STREQUAL "")
		set(FBSTRING_PluginName "${FBSTRING_PluginName} ${FBSTRING_V_TXT_X11}")
	endif(NOT ${FBSTRING_V_TXT_X11} STREQUAL "")
endif(FB_PLATFORM_NAME STREQUAL "X11") 
if(FB_PLATFORM_ARCH_32)
	if(NOT ${FBSTRING_V_TXT_X86} STREQUAL "")
		set(FBSTRING_PluginName "${FBSTRING_PluginName} ${FBSTRING_V_TXT_X86}")
	endif(NOT ${FBSTRING_V_TXT_X86} STREQUAL "")
	if(NOT ${FBSTRING_V_X86} STREQUAL "")
		set(FBSTRING_PluginDomain "${FBSTRING_PluginDomain}-${FBSTRING_V_X86}")
		set(FBSTRING_PluginFileName "${FBSTRING_PluginFileName}-${FBSTRING_V_X86}")
	endif(NOT ${FBSTRING_V_X86} STREQUAL "")
endif(FB_PLATFORM_ARCH_32)
if(FB_PLATFORM_ARCH_64)
	if(NOT ${FBSTRING_V_TXT_X86_64} STREQUAL "")
		set(FBSTRING_PluginName "${FBSTRING_PluginName} ${FBSTRING_V_TXT_X86_64}")
	endif(NOT ${FBSTRING_V_TXT_X86_64} STREQUAL "")
	if(NOT ${FBSTRING_V_X86_64} STREQUAL "")
		set(FBSTRING_PluginDomain "${FBSTRING_PluginDomain}-${FBSTRING_V_X86_64}")
		set(FBSTRING_PluginFileName "${FBSTRING_PluginFileName}-${FBSTRING_V_X86_64}")
	endif(NOT ${FBSTRING_V_X86_64} STREQUAL "")
endif(FB_PLATFORM_ARCH_64)

message("-- Platform: ${FBSTRING_PluginName}")

### PACKAGES ###
# WiX
if(FB_PLATFORM_ARCH_32)
	set(FBControl_WixUpgradeCode_GUID d04684cd-332b-540f-beca-53826c14d6cf)
else(FB_PLATFORM_ARCH_32)
	set(FBControl_WixUpgradeCode_GUID d04684cd-332b-540f-beca-53826c14d6df)
endif(FB_PLATFORM_ARCH_32)
# Set to ON for machine-wide installation and to OFF for user-local installation on Windows
set(FB_ATLREG_MACHINEWIDE OFF)

# XPI
set(FBControl_XPI_GUID_WIN_32 "{A7F0040E-4481-4C03-952F-D8491E5036A3}")
set(FBControl_XPI_GUID_WIN_64 "{A7F0040E-4481-4C03-952F-D8491E5036A4}")
set(FBControl_XPI_GUID_MAC_32 "{A7F0040E-4481-4C03-952F-D8491E5036A5}")
set(FBControl_XPI_GUID_MAC_64 "{A7F0040E-4481-4C03-952F-D8491E5036A6}")
set(FBControl_XPI_GUID_X11_32 "{A7F0040E-4481-4C03-952F-D8491E5036A7}")
set(FBControl_XPI_GUID_X11_64 "{A7F0040E-4481-4C03-952F-D8491E5036A8}")
if(FB_PLATFORM_NAME STREQUAL "Win")
	if(FB_PLATFORM_ARCH_32)
		set(FBControl_XPI_GUID ${FBControl_XPI_GUID_WIN_32})
	else(FB_PLATFORM_ARCH_32)
		set(FBControl_XPI_GUID ${FBControl_XPI_GUID_WIN_64})
	endif(FB_PLATFORM_ARCH_32)
endif(FB_PLATFORM_NAME STREQUAL "Win") 
if(FB_PLATFORM_NAME STREQUAL "Mac")
	if(FB_PLATFORM_ARCH_32)
		set(FBControl_XPI_GUID ${FBControl_XPI_GUID_MAC_32})
	else(FB_PLATFORM_ARCH_32)
		set(FBControl_XPI_GUID ${FBControl_XPI_GUID_MAC_64})
	endif(FB_PLATFORM_ARCH_32)
endif(FB_PLATFORM_NAME STREQUAL "Mac") 
if(FB_PLATFORM_NAME STREQUAL "X11")
	if(FB_PLATFORM_ARCH_32)
		set(FBControl_XPI_GUID ${FBControl_XPI_GUID_X11_32})
	else(FB_PLATFORM_ARCH_32)
		set(FBControl_XPI_GUID ${FBControl_XPI_GUID_X11_64})
	endif(FB_PLATFORM_ARCH_32)
endif(FB_PLATFORM_NAME STREQUAL "X11") 

#################


### SIGNING ###

set(LW_PFX_FILENAME "linphoneweb.pfx")
set(LW_PASSPHRASE_FILENAME "passphrase.txt")
set(LW_TIMESTAMP_URL "http://timestamp.verisign.com/scripts/timestamp.dll")


### CONFIGURATIONS ###

option(LW_CREATE_CRX "Enable creation of a CRX package of linphoneweb" ON)
option(LW_CREATE_XPI "Enable creation of a XPI package of linphoneweb" ON)
option(LW_CREATE_SDK "Enable creation of a SDK of linphoneweb" ON)
option(LW_DEBUG_GENERATE_DUMPS "Enable creation of dumps when the linphoneweb crashes on the Windows platform" OFF)


######################

# Uncomment this next line if you're not planning on your plugin doing
# any drawing:

#set (FB_GUI_DISABLED 1)

# Mac plugin settings. If your plugin does not draw, set these all to 0
set(FBMAC_USE_QUICKDRAW 0)
set(FBMAC_USE_CARBON 0)
set(FBMAC_USE_COCOA 1)
set(FBMAC_USE_COREGRAPHICS 0)
set(FBMAC_USE_COREANIMATION 1)
set(FBMAC_USE_INVALIDATINGCOREANIMATION 1)

# Force use of filesystem v3
set(BOOST_FILESYSTEM_V3 1)

add_boost_library(filesystem)
if(CMAKE_BUILD_TYPE MATCHES Debug)
	add_firebreath_library(log4cplus)
endif()
