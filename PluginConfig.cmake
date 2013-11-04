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

# Project build properties
set(PLUGIN_NAME "linphone-web")
set(PLUGIN_PREFIX "LINWEB")
set(COMPANY_NAME "belledonne-communications")
set(PLUGIN_SHAREDIR "linphoneweb")
set(CORE_ROOTFS_VERSION "all_codecs-32d48f2eed30f30754f0a0c5c32745de0a1a91ec")
set(CORE_ROOTFS_URL "http://www.linphone.org/snapshots/linphone-web/rootfs/linphone-web-rootfs")

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

set(FBSTRING_PLUGIN_VERSION "0.0.2.10")
set(FBSTRING_PLUGIN_VERSION_ACTIVEX "0,0,2,10")
set(FBSTRING_LegalCopyright "Copyright 2013 Belledonne Communications")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}")

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
set(FBSTRING_MIMEType 
	"application/x-linphone-web"
	"application/x-linphone-web-video")
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
		set(CORE_ROOTFS_URL "${CORE_ROOTFS_URL}-windows")
	endif(NOT ${FBSTRING_V_TXT_WIN} STREQUAL "")
endif(FB_PLATFORM_NAME STREQUAL "Win") 
if(FB_PLATFORM_NAME STREQUAL "Mac")
	if(NOT ${FBSTRING_V_TXT_MAC} STREQUAL "")
		set(FBSTRING_PluginName "${FBSTRING_PluginName} ${FBSTRING_V_TXT_MAC}")
		set(CORE_ROOTFS_URL "${CORE_ROOTFS_URL}-darwin")
	endif(NOT ${FBSTRING_V_TXT_MAC} STREQUAL "")
endif(FB_PLATFORM_NAME STREQUAL "Mac") 
if(FB_PLATFORM_NAME STREQUAL "X11")
	if(NOT ${FBSTRING_V_TXT_X11} STREQUAL "")
		set(FBSTRING_PluginName "${FBSTRING_PluginName} ${FBSTRING_V_TXT_X11}")
		set(CORE_ROOTFS_URL "${CORE_ROOTFS_URL}-linux")
	endif(NOT ${FBSTRING_V_TXT_X11} STREQUAL "")
endif(FB_PLATFORM_NAME STREQUAL "X11") 
if(FB_PLATFORM_ARCH_32)
	if(NOT ${FBSTRING_V_TXT_X86} STREQUAL "")
		set(FBSTRING_PluginName "${FBSTRING_PluginName} ${FBSTRING_V_TXT_X86}")
	endif(NOT ${FBSTRING_V_TXT_X86} STREQUAL "")
	if(NOT ${FBSTRING_V_X86} STREQUAL "")
		set(FBSTRING_PluginDomain "${FBSTRING_PluginDomain}-${FBSTRING_V_X86}")
		set(FBSTRING_PluginFileName "${FBSTRING_PluginFileName}${FBSTRING_V_X86}")
	endif(NOT ${FBSTRING_V_X86} STREQUAL "")
	set(CORE_ROOTFS_URL "${CORE_ROOTFS_URL}-x86")
endif(FB_PLATFORM_ARCH_32)
if(FB_PLATFORM_ARCH_64)
	if(NOT ${FBSTRING_V_TXT_X86_64} STREQUAL "")
		set(FBSTRING_PluginName "${FBSTRING_PluginName} ${FBSTRING_V_TXT_X86_64}")
	endif(NOT ${FBSTRING_V_TXT_X86_64} STREQUAL "")
	if(NOT ${FBSTRING_V_X86_64} STREQUAL "")
		set(FBSTRING_PluginDomain "${FBSTRING_PluginDomain}-${FBSTRING_V_X86_64}")
		set(FBSTRING_PluginFileName "${FBSTRING_PluginFileName}${FBSTRING_V_X86_64}")
	endif(NOT ${FBSTRING_V_X86_64} STREQUAL "")
	set(CORE_ROOTFS_URL "${CORE_ROOTFS_URL}-x86_64")
endif(FB_PLATFORM_ARCH_64)
set(CORE_ROOTFS_URL "${CORE_ROOTFS_URL}-${CORE_ROOTFS_VERSION}.tar.gz")

message("-- Platform: ${FBSTRING_PluginName}")
message("-- Core Rootfs URL: ${CORE_ROOTFS_URL}")

### PACKAGES ###
# WiX
if(FB_PLATFORM_ARCH_32)
	set(FBControl_WixUpgradeCode_GUID d04684cd-332b-540f-beca-53826c14d6cf)
else(FB_PLATFORM_ARCH_32)
	set(FBControl_WixUpgradeCode_GUID d04684cd-332b-540f-beca-53826c14d6df)
endif(FB_PLATFORM_ARCH_32)

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


### CONFIGURATIONS ###

set(LW_USE_SRTP 1)
set(LW_USE_FFMPEG 1)
set(LW_USE_G729 1)
set(LW_USE_X264 1)
set(LW_USE_OPENSSL 0)
set(LW_USE_EXOSIP 0)
set(LW_USE_POLARSSL 1)
set(LW_USE_BELLESIP 1)

##
message("-- Configurations:")
if(LW_USE_SRTP)
message("    + SRTP")
endif(LW_USE_SRTP)
if(LW_USE_FFMPEG)
message("    + FFMPEG")
endif(LW_USE_FFMPEG)
if(LW_USE_G729)
message("    + G729")
endif(LW_USE_G729)
if(LW_USE_X264)
message("    + X264")
endif(LW_USE_X264)
if(LW_USE_OPENSSL)
message("    + OPENSSL")
endif(LW_USE_OPENSSL)
if(LW_USE_EXOSIP)
message("    + EXOSIP")
endif(LW_USE_EXOSIP)
if(LW_USE_POLARSSL)
message("    + POLARSSL")
endif(LW_USE_POLARSSL)
if(LW_USE_BELLESIP)
message("    + BELLESIP")
endif(LW_USE_BELLESIP)

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

# If you want to register per-machine on Windows, uncomment this line
#set (FB_ATLREG_MACHINEWIDE 1)

# Force use of filesystem v3
set(BOOST_FILESYSTEM_V3 1)

add_boost_library(filesystem)
if(CMAKE_BUILD_TYPE MATCHES Debug)
	add_firebreath_library(log4cplus)
endif()
