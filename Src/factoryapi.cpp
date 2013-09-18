/*!
 Linphone Web - Web plugin of Linphone an audio/video SIP phone
 Copyright (C) 2012-2013 Belledonne Communications
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 
 
 Authors:
 - Yann Diorcet <diorcet.yann@gmail.com>
 
 */

#include "factoryapi.h"
#include "lpconfigapi.h"

#include <exception>

namespace LinphoneWeb {

FactoryAPI::FactoryAPI(FB::PluginCorePtr const &plugin, WhiteBoardPtr const &whiteboard): mPlugin(plugin), mWhiteBoard(whiteboard) {
	FBLOG_DEBUG("FactoryAPI::FactoryAPI", "this=" << this);
}

FactoryAPI::~FactoryAPI() {
	FBLOG_DEBUG("FactoryAPI::~FactoryAPI", "this=" << this);
}

FB::PluginCorePtr FactoryAPI::getPlugin() const {
	FB::PluginCorePtr plugin(mPlugin.lock());
	if (!plugin) {
		throw FB::script_error("The plugin is invalid");
	}
	return plugin;
}

WhiteBoardPtr FactoryAPI::getWhiteBoard() const {
	return mWhiteBoard;
}

boost::recursive_mutex &FactoryAPI::getCoreMutex() const {
	return mCoreMutex;
}

AddressAPIPtr FactoryAPI::getAddress(LinphoneAddress *address) {
	FBLOG_DEBUG("FactoryAPI::getAddress", "this=" << this << "\t" << "address=" << address);
	if (address == NULL)
		return AddressAPIPtr();
	
	AddressAPIPtr shared_ptr;
	try {
		shared_ptr = AddressAPIPtr(new AddressAPI(address));
		handle(shared_ptr, false, false);
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getAddress", "exception: " << e.what());
	}
	return shared_ptr;
}

AddressAPIPtr FactoryAPI::getAddress(const LinphoneAddress *address) {
	FBLOG_DEBUG("FactoryAPI::getAddress", "this=" << this << "\t" << "address=" << address);
	if (address == NULL)
		return AddressAPIPtr();
	
	AddressAPIPtr shared_ptr;
	try {
		shared_ptr = AddressAPIPtr(new AddressAPI(address));
		handle(shared_ptr, false, true);
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getAddress", "exception: " << e.what());
	}
	return shared_ptr;
}
	
AddressAPIPtr FactoryAPI::getAddress(StringPtr const &address) {
	FBLOG_DEBUG("FactoryAPI::getAddress", "this=" << this << "\t" << "address=" << address);
	
	AddressAPIPtr shared_ptr;
	try {
		shared_ptr = AddressAPIPtr(new AddressAPI(address));
		handle(shared_ptr, true, false);
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getAddress", "exception: " << e.what());
	}
	return shared_ptr;
}

AuthInfoAPIPtr FactoryAPI::getAuthInfo(LinphoneAuthInfo *authInfo) {
	FBLOG_DEBUG("FactoryAPI::getAuthInfo", "this=" << this << "\t" << "authInfo=" << authInfo);
	if (authInfo == NULL)
		return AuthInfoAPIPtr();
	
	AuthInfoAPIPtr shared_ptr;
	try {
		shared_ptr = AuthInfoAPIPtr(new AuthInfoAPI(authInfo));
		handle(shared_ptr, false, false);
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getAuthInfo", "exception: " << e.what());
	}
	return shared_ptr;
}

AuthInfoAPIPtr FactoryAPI::getAuthInfo(const LinphoneAuthInfo *authInfo) {
	FBLOG_DEBUG("FactoryAPI::getAuthInfo", "this=" << this << "\t" << "authInfo=" << authInfo);
	if (authInfo == NULL)
		return AuthInfoAPIPtr();
	
	AuthInfoAPIPtr shared_ptr;
	try{
		shared_ptr = AuthInfoAPIPtr(new AuthInfoAPI(authInfo));
		handle(shared_ptr, false, true);
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getAuthInfo", "exception: " << e.what());
	}
	return shared_ptr;
}

AuthInfoAPIPtr FactoryAPI::getAuthInfo(StringPtr const &username, StringPtr const &userid,
						   StringPtr const &passwd, StringPtr const &ha1, StringPtr const &realm) {
	FBLOG_DEBUG("FactoryAPI::getAuthInfo", "this=" << this << "\t" << "username=" << username
				<< "\t" << "userid=" << userid
				<< "\t" << "passwd=" << passwd
				<< "\t" << "ha1=" << ha1
				<< "\t" << "realm=" << realm);
	
	AuthInfoAPIPtr shared_ptr;
	try {
		shared_ptr = AuthInfoAPIPtr(new AuthInfoAPI(username, userid, passwd, ha1, realm));
		handle(shared_ptr, true, false);
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getAuthInfo", "exception: " << e.what());
	}
	return shared_ptr;
}
	

CallAPIPtr FactoryAPI::getCall(LinphoneCall *call) {
	FBLOG_DEBUG("FactoryAPI::getCall", "this=" << this << "\t" << "call=" << call);
	if (call == NULL)
		return CallAPIPtr();
	
	void *ptr = linphone_call_get_user_pointer(call);
	CallAPIPtr shared_ptr;
	try {
		if (ptr == NULL) {
			shared_ptr = CallAPIPtr(new CallAPI(call));
			handle(shared_ptr, false, false);
		} else {
			shared_ptr = boost::static_pointer_cast<CallAPI>(reinterpret_cast<CallAPI *>(ptr)->shared_from_this());
		}
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getCall", "exception: " << e.what());
	}
	return shared_ptr;
}

CallLogAPIPtr FactoryAPI::getCallLog(LinphoneCallLog *callLog) {
	FBLOG_DEBUG("FactoryAPI::getCallLog", "this=" << this << "\t" << "callLog=" << callLog);
	if (callLog == NULL)
		return CallLogAPIPtr();
	
	void *ptr = linphone_call_log_get_user_pointer(callLog);
	CallLogAPIPtr shared_ptr;
	try {
		if (ptr == NULL) {
			shared_ptr = CallLogAPIPtr(new CallLogAPI(callLog));
			handle(shared_ptr, false, false);
		} else {
			shared_ptr = boost::static_pointer_cast<CallLogAPI>(reinterpret_cast<CallLogAPI *>(ptr)->shared_from_this());
		}
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getCallLog", "exception: " << e.what());
	}
	return shared_ptr;
}

CallParamsAPIPtr FactoryAPI::getCallParams(LinphoneCallParams *callParams) {
	FBLOG_DEBUG("FactoryAPI::getCallParams", "this=" << this << "\t" << "callParams=" << callParams);
	if (callParams == NULL)
		return CallParamsAPIPtr();
	
	CallParamsAPIPtr shared_ptr;
	try {
		shared_ptr = CallParamsAPIPtr(new CallParamsAPI(callParams));
		handle(shared_ptr, false, false);
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getCallParams", "exception: " << e.what());
	}
	return shared_ptr;
}

CallParamsAPIPtr FactoryAPI::getCallParams(const LinphoneCallParams *callParams) {
	FBLOG_DEBUG("FactoryAPI::getCallParams", "this=" << this << "\t" << "callParams=" << callParams);
	if (callParams == NULL)
		return CallParamsAPIPtr();
	
	CallParamsAPIPtr shared_ptr;
	try {
		shared_ptr = CallParamsAPIPtr(new CallParamsAPI(callParams));
		handle(shared_ptr, false, true);
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getCallParams", "exception: " << e.what());
	}
	return shared_ptr;
}

CallStatsAPIPtr FactoryAPI::getCallStats(const LinphoneCallStats *callStats) {
	FBLOG_DEBUG("FactoryAPI::getCallStats", "this=" << this << "\t" << "callStats=" << callStats);
	if (callStats == NULL)
		return CallStatsAPIPtr();
	
	CallStatsAPIPtr shared_ptr;
	try {
		shared_ptr = CallStatsAPIPtr(new CallStatsAPI(callStats));
		handle(shared_ptr, false, true);
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getCallStats", "exception: " << e.what());
	}
	return shared_ptr;
}

CoreAPIPtr FactoryAPI::getCore(LinphoneCore *core) {
	FBLOG_DEBUG("FactoryAPI::getCore", "this=" << this << "\t" << "core=" << core);
	if (core == NULL) {
		CoreAPIPtr shared_ptr = CoreAPIPtr(new CoreAPI());
		return handle(shared_ptr, false, false); /* Not really owned (keep to false in order lock mutex on functions call) */
	}
	
	void *ptr = linphone_core_get_user_data(core);
	CoreAPIPtr shared_ptr;
	try {
		if (ptr != NULL) {
			shared_ptr = boost::static_pointer_cast<CoreAPI>(reinterpret_cast<CoreAPI *>(ptr)->shared_from_this());
		}
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getCore", "exception: " << e.what());
	}
	return shared_ptr;
}

FriendAPIPtr FactoryAPI::getFriend(LinphoneFriend *f) {
	FBLOG_DEBUG("FactoryAPI::getFriend", "this=" << this << "\t" << "f=" << f);
	if (f == NULL)
		return FriendAPIPtr();

	FriendAPIPtr shared_ptr;
	try {
		shared_ptr = FriendAPIPtr(new FriendAPI(f));
		handle(shared_ptr, false, false);
	} catch (std::exception &e) {
		FBLOG_WARN("FactoryAPI::getFriend", "exception: " << e.what());
	}
	return shared_ptr;
}

FriendAPIPtr FactoryAPI::getFriend(StringPtr const &address) {
	FBLOG_DEBUG("FactoryAPI::getFriend", "this=" << this << "\t" << "address=" << address);

	FriendAPIPtr shared_ptr;
	try {
		shared_ptr = FriendAPIPtr(new FriendAPI(address));
		handle(shared_ptr, true, false);
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getFriend", "exception: " << e.what());
	}
	return shared_ptr;
}

LpConfigAPIPtr FactoryAPI::getLpConfig(LpConfig *config) {
	FBLOG_DEBUG("FactoryAPI::getLpConfig", "this=" << this << "\t" << "config=" << config);
	if (config == NULL)
		return LpConfigAPIPtr();

	LpConfigAPIPtr shared_ptr;
	try {
		shared_ptr = LpConfigAPIPtr(new LpConfigAPI(config));
		handle(shared_ptr, false, false);
	} catch (std::exception &e) {
		FBLOG_WARN("FactoryAPI::getLpConfig", "exception: " << e.what());
	}
	return shared_ptr;
}

LpConfigAPIPtr FactoryAPI::getLpConfig(StringPtr const &configFilename) {
	FBLOG_DEBUG("FactoryAPI::getLpConfig", "this=" << this << "\t" << "configFilename=" << configFilename);

	LpConfigAPIPtr shared_ptr;
	try {
		shared_ptr = LpConfigAPIPtr(new LpConfigAPI(configFilename));
		handle(shared_ptr, true, false);
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getLpConfig", "exception: " << e.what());
	}
	return shared_ptr;
}

PayloadTypeAPIPtr FactoryAPI::getPayloadType(PayloadType *payloadType) {
	FBLOG_DEBUG("FactoryAPI::getPayloadType", "this=" << this << "\t" << "payloadType=" << payloadType);
	if (payloadType == NULL)
		return PayloadTypeAPIPtr();
	
	//void *ptr = payloadType->user_data;
	PayloadTypeAPIPtr shared_ptr;
	try {
		/*if (ptr == NULL) {*/
		shared_ptr = PayloadTypeAPIPtr(new PayloadTypeAPI(payloadType));
		/*} else {
		 shared_ptr = boost::static_pointer_cast<PayloadTypeAPI>(reinterpret_cast<PayloadTypeAPI *>(ptr)->shared_from_this());
		 }*/
		handle(shared_ptr, false, false);
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getPayloadType", "exception: " << e.what());
	}
	return shared_ptr;
}

PayloadTypeAPIPtr FactoryAPI::getPayloadType(const PayloadType *payloadType) {
	FBLOG_DEBUG("FactoryAPI::getPayloadType", "this=" << this << "\t" << "payloadType=" << payloadType);
	if (payloadType == NULL)
		return PayloadTypeAPIPtr();
	
	//void *ptr = payloadType->user_data;
	PayloadTypeAPIPtr shared_ptr;
	try {
		/*if (ptr == NULL) {*/
		shared_ptr = PayloadTypeAPIPtr(new PayloadTypeAPI(payloadType));
		/*} else {
		shared_ptr = boost::static_pointer_cast<PayloadTypeAPI>(reinterpret_cast<PayloadTypeAPI *>(ptr)->shared_from_this());
		}*/
		handle(shared_ptr, false, true);
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getPayloadType", "exception: " << e.what());
	}
	return shared_ptr;
}

ProxyConfigAPIPtr FactoryAPI::getProxyConfig(LinphoneProxyConfig *proxyConfig) {
	FBLOG_DEBUG("FactoryAPI::getProxyConfig", "this=" << this << "\t" << "proxyConfig=" << proxyConfig);
	if (proxyConfig == NULL)
		return ProxyConfigAPIPtr();
	
	void *ptr = linphone_proxy_config_get_user_data(proxyConfig);
	ProxyConfigAPIPtr shared_ptr;
	try {
		if (ptr == NULL) {
			shared_ptr = ProxyConfigAPIPtr(new ProxyConfigAPI(proxyConfig));
			handle(shared_ptr, false, false);
		} else {
			shared_ptr = boost::static_pointer_cast<ProxyConfigAPI>(reinterpret_cast<ProxyConfigAPI *>(ptr)->shared_from_this());
		}
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getProxyConfig", "exception: " << e.what());
	}
	return shared_ptr;
}
	
ProxyConfigAPIPtr FactoryAPI::getProxyConfig() {
	FBLOG_DEBUG("FactoryAPI::getProxyConfig", "this=" << this);
	ProxyConfigAPIPtr shared_ptr;
	try{
		shared_ptr = ProxyConfigAPIPtr(new ProxyConfigAPI());
		handle(shared_ptr, true, false);
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getProxyConfig", "exception: " << e.what());
	}
	return shared_ptr;
}

SipTransportsAPIPtr FactoryAPI::getSipTransports() {
	FBLOG_DEBUG("FactoryAPI::getSipTransports", "this=" << this);
	SipTransportsAPIPtr shared_ptr;
	try{
		shared_ptr = SipTransportsAPIPtr(new SipTransportsAPI());
		handle(shared_ptr, true, false);
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getSipTransports", "exception: " << e.what());
	}
	return shared_ptr;
}

VideoAPIPtr FactoryAPI::getVideo() {
	FBLOG_DEBUG("FactoryAPI::getVideo", "this=" << this);
	VideoAPIPtr shared_ptr;
	try{
		shared_ptr = VideoAPIPtr(new VideoAPI());
		handle(shared_ptr, true, false);
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getVideo", "exception: " << e.what());
	}
	return shared_ptr;
}
	
VideoPolicyAPIPtr FactoryAPI::getVideoPolicy() {
	FBLOG_DEBUG("FactoryAPI::getVideoPolicy", "this=" << this);
	VideoPolicyAPIPtr shared_ptr;
	try {
		shared_ptr = VideoPolicyAPIPtr(new VideoPolicyAPI());
		handle(shared_ptr, true, false);
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getVideoPolicy", "exception: " << e.what());
	}
	return shared_ptr;
}

FileManagerAPIPtr FactoryAPI::getFileManager() {
	FBLOG_DEBUG("FactoryAPI::getFileManager", "this=" << this);
	FileManagerAPIPtr shared_ptr = mFileManager.lock();
	if(!shared_ptr) {
		try {
			shared_ptr = FileManagerAPIPtr(new FileManagerAPI());
			handle(shared_ptr, true, false);
			mFileManager = shared_ptr;
		} catch(std::exception &e) {
			FBLOG_WARN("FactoryAPI::getFileManager", "exception: " << e.what());
		}
	}
	return shared_ptr;
}

FileTransferAPIPtr FactoryAPI::getFileTransfer(FB::URI const &sourceUri, FB::URI const &targetUri, FB::JSObjectPtr const &callback) {
	FBLOG_DEBUG("FactoryAPI::getFileTransfer", "this=" << this
				<< "\t" << "sourceUri=" << sourceUri.toString()
				<< "\t" << "targetUri=" << targetUri.toString()
				<< "\t" << "callback=" << callback);
	FileTransferAPIPtr shared_ptr;
	try {
		shared_ptr = FileTransferAPI::create(sourceUri, targetUri, callback);
		handle(shared_ptr, true, false);
	} catch(std::exception &e) {
		FBLOG_WARN("FactoryAPI::getFileTransfer", "exception: " << e.what());
	}
	return shared_ptr;
}

} // LinphoneWeb
