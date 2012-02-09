#/**********************************************************\ 
#
# Auto-Generated Plugin Configuration file
# for linphone
#
#\**********************************************************/

set(PLUGIN_NAME "linphone")
set(PLUGIN_PREFIX "LIN")
set(COMPANY_NAME "belledonne_communications")

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
set(FBControl_GUID 1563fddd-83a0-553a-854b-a6f75ed2d5f6)
set(IFBComJavascriptObject_GUID fa963671-1cb5-537b-baa8-d3b60c13e09c)
set(FBComJavascriptObject_GUID b4228fbd-ed98-5619-a71b-93b3e4fe422d)
set(IFBComEventSource_GUID 898a611e-b63b-58c7-bc64-0c0fa0c0cded)

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "belledonne_communications.linphone")
set(MOZILLA_PLUGINID "belledonne-communications.com/linphone")

# strings
set(FBSTRING_CompanyName "Belledonne communications")
set(FBSTRING_FileDescription "Linphone Plugin")
set(FBSTRING_PLUGIN_VERSION "1.0.0.0")
set(FBSTRING_LegalCopyright "Copyright 2012 Belledonne communications")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}.dll")
set(FBSTRING_ProductName "linphone")
set(FBSTRING_FileExtents "")
set(FBSTRING_PluginName "linphone")
set(FBSTRING_MIMEType "application/x-linphone")

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
