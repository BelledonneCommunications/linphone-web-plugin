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

set(PLUGIN_NAME "linphone-web")
set(PLUGIN_PREFIX "LINWEB")
set(COMPANY_NAME "belledonne-communications")

# ActiveX constants:
set(FBTYPELIB_NAME linphoneLib)
set(FBTYPELIB_DESC "linphone 1.0 Type Library")
set(IFBControl_DESC "linphone Control Interface")
set(FBControl_DESC "linphone Control Class")
set(IFBComJavascriptObject_DESC "linphone IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "linphone ComJavascriptObject Class")
set(IFBComEventSource_DESC "linphone IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID 65fd31b0-c3b6-5741-b010-c033e6766a1d)
set(IFBControl_GUID 34538382-721c-5b50-be7f-a2b384eb36c6)
set(IFBComJavascriptObject_GUID fa963671-1cb5-537b-baa8-d3b60c13e09c)
set(FBComJavascriptObject_GUID b4228fbd-ed98-5619-a71b-93b3e4fe422d)
set(IFBComEventSource_GUID 898a611e-b63b-58c7-bc64-0c0fa0c0cded)

# these are the pieces that are relevant to using it from Javascript
set(MOZILLA_PLUGINID "belledonne-communications.com/linphone/web")

# strings
set(FBSTRING_CompanyName "Belledonne Communications")
set(FBSTRING_PLUGIN_VERSION "1.0.0.0")
set(FBSTRING_LegalCopyright "Copyright 2012 Belledonne Communications")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}.dll")
set(FBSTRING_ProductName "Linphone Web")
set(FBSTRING_FileExtents "")
set(FBSTRING_PluginName "Linphone Web")

set(FBSTRING_MIMEType 
	"application/x-linphone-web"
	"application/x-linphone-web-video")
set(ACTIVEX_PROGID 
	"com.belledonne-communications.linphone.web"
	"com.belledonne-communications.linphone.web.video")
set(FBControl_GUID 
	1563fddd-83a0-553a-854b-a6f75ed2d5f6
	1563fddd-83a0-553a-854b-a6f75ed2d5f7)
set(FBSTRING_FileDescription 
	"Linphone Web Plugin"
	"Linphone Web Video Plugin")

# Uncomment this next line if you're not planning on your plugin doing
# any drawing:

#set (FB_GUI_DISABLED 1)

# Mac plugin settings. If your plugin does not draw, set these all to 0
set(FBMAC_USE_QUICKDRAW 0)
set(FBMAC_USE_CARBON 1)
set(FBMAC_USE_COCOA 1)
set(FBMAC_USE_COREGRAPHICS 1)
set(FBMAC_USE_COREANIMATION 0)
set(FBMAC_USE_INVALIDATINGCOREANIMATION 0)

# If you want to register per-machine on Windows, uncomment this line
#set (FB_ATLREG_MACHINEWIDE 1)

if(CMAKE_BUILD_TYPE MATCHES Debug)
	add_firebreath_library(log4cplus)
endif()
