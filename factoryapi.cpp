/*!
 Linphone Web - Web plugin of Linphone an audio/video SIP phone
 Copyright (C) 2012  Yann Diorcet <yann.diorcet@linphone.org>
 
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
 */

#include "factoryapi.h"
#include "Transfers/downloadfiletransferapi.h"
#include "Transfers/uploadfiletransferapi.h"
#include "Transfers/localfiletransferapi.h"

namespace LinphoneWeb {

FactoryAPI::FactoryAPI(const CorePluginWeakPtr &plugin): mPlugin(plugin) {
	FBLOG_DEBUG("FactoryAPI::FactoryAPI", "this=" << this);
}

FactoryAPI::~FactoryAPI() {
	FBLOG_DEBUG("FactoryAPI::~FactoryAPI", "this=" << this);
}

CorePluginPtr FactoryAPI::getPlugin() {
	CorePluginPtr plugin(mPlugin.lock());
	if (!plugin) {
		throw FB::script_error("The plugin is invalid");
	}
	return plugin;
}

boost::recursive_mutex &FactoryAPI::getCoreMutex() {
	return mCoreMutex;
}

AddressAPIPtr FactoryAPI::getAddress(LinphoneAddress *address) {
	FBLOG_DEBUG("FactoryAPI::getAddress", "this=" << this << "\t" << "address=" << address);
	if (address == NULL)
		return AddressAPIPtr();
	
	AddressAPIPtr shared_ptr;
	shared_ptr = AddressAPIPtr(new AddressAPI(address));
	return get(shared_ptr, false, false);
}

AddressAPIPtr FactoryAPI::getAddress(const LinphoneAddress *address) {
	FBLOG_DEBUG("FactoryAPI::getAddress", "this=" << this << "\t" << "address=" << address);
	if (address == NULL)
		return AddressAPIPtr();
	
	AddressAPIPtr shared_ptr;
	shared_ptr = AddressAPIPtr(new AddressAPI(address));
	return get(shared_ptr, false, true);
}

AuthInfoAPIPtr FactoryAPI::getAuthInfo(LinphoneAuthInfo *authInfo) {
	FBLOG_DEBUG("FactoryAPI::getAuthInfo", "this=" << this << "\t" << "authInfo=" << authInfo);
	if (authInfo == NULL)
		return AuthInfoAPIPtr();
	
	AuthInfoAPIPtr shared_ptr;
	shared_ptr = AuthInfoAPIPtr(new AuthInfoAPI(authInfo));
	return get(shared_ptr, false, false);
}

AuthInfoAPIPtr FactoryAPI::getAuthInfo(const LinphoneAuthInfo *authInfo) {
	FBLOG_DEBUG("FactoryAPI::getAuthInfo", "this=" << this << "\t" << "authInfo=" << authInfo);
	if (authInfo == NULL)
		return AuthInfoAPIPtr();
	
	AuthInfoAPIPtr shared_ptr;
	shared_ptr = AuthInfoAPIPtr(new AuthInfoAPI(authInfo));
	return get(shared_ptr, false, true);
}

AuthInfoAPIPtr FactoryAPI::getAuthInfo(const StringPtr &username, const StringPtr &userid,
						   const StringPtr &passwd, const StringPtr &ha1, const StringPtr &realm) {
	FBLOG_DEBUG("FactoryAPI::getAuthInfo", "this=" << this << "\t" << "username=" << username
				<< "\t" << "userid=" << userid
				<< "\t" << "passwd=" << passwd
				<< "\t" << "ha1=" << ha1
				<< "\t" << "realm=" << realm);
	
	AuthInfoAPIPtr shared_ptr;
	shared_ptr = AuthInfoAPIPtr(new AuthInfoAPI(username, userid, passwd, ha1, realm));
	return get(shared_ptr, true, false);
}
	

CallAPIPtr FactoryAPI::getCall(LinphoneCall *call) {
	FBLOG_DEBUG("FactoryAPI::getCall", "this=" << this << "\t" << "call=" << call);
	if (call == NULL)
		return CallAPIPtr();
	
	void *ptr = linphone_call_get_user_pointer(call);
	CallAPIPtr shared_ptr;
	if (ptr == NULL) {
		shared_ptr = CallAPIPtr(new CallAPI(call));
		get(shared_ptr, false, false);
	} else {
		shared_ptr = boost::static_pointer_cast<CallAPI>(reinterpret_cast<CallAPI *>(ptr)->shared_from_this());
	}
	return shared_ptr;
}

CallLogAPIPtr FactoryAPI::getCallLog(LinphoneCallLog *callLog) {
	FBLOG_DEBUG("FactoryAPI::getCallLog", "this=" << this << "\t" << "callLog=" << callLog);
	if (callLog == NULL)
		return CallLogAPIPtr();
	
	void *ptr = linphone_call_log_get_user_pointer(callLog);
	CallLogAPIPtr shared_ptr;
	if (ptr == NULL) {
		shared_ptr = CallLogAPIPtr(new CallLogAPI(callLog));
		get(shared_ptr, false, false);
	} else {
		shared_ptr = boost::static_pointer_cast<CallLogAPI>(reinterpret_cast<CallLogAPI *>(ptr)->shared_from_this());
	}
	return shared_ptr;
}

CallParamsAPIPtr FactoryAPI::getCallParams(LinphoneCallParams *callParams) {
	FBLOG_DEBUG("FactoryAPI::getCallParams", "this=" << this << "\t" << "callParams=" << callParams);
	if (callParams == NULL)
		return CallParamsAPIPtr();
	
	CallParamsAPIPtr shared_ptr;
	shared_ptr = CallParamsAPIPtr(new CallParamsAPI(callParams));
	return get(shared_ptr, false, false);
}

CallParamsAPIPtr FactoryAPI::getCallParams(const LinphoneCallParams *callParams) {
	FBLOG_DEBUG("FactoryAPI::getCallParams", "this=" << this << "\t" << "callParams=" << callParams);
	if (callParams == NULL)
		return CallParamsAPIPtr();
	
	CallParamsAPIPtr shared_ptr;
	shared_ptr = CallParamsAPIPtr(new CallParamsAPI(callParams));
	return get(shared_ptr, false, true);
}

CallStatsAPIPtr FactoryAPI::getCallStats(const LinphoneCallStats *callStats) {
	FBLOG_DEBUG("FactoryAPI::getCallStats", "this=" << this << "\t" << "callStats=" << callStats);
	if (callStats == NULL)
		return CallStatsAPIPtr();
	
	CallStatsAPIPtr shared_ptr;
	shared_ptr = CallStatsAPIPtr(new CallStatsAPI(callStats));
	return get(shared_ptr, false, true);
}

CoreAPIPtr FactoryAPI::getCore(LinphoneCore *core) {
	FBLOG_DEBUG("FactoryAPI::getCore", "this=" << this << "\t" << "core=" << core);
	if (core == NULL) {
		CoreAPIPtr shared_ptr = CoreAPIPtr(new CoreAPI());
		return get(shared_ptr, false, false); /* Not really owned (keep to false in order lock mutex on functions call) */
	}
	
	void *ptr = linphone_core_get_user_data(core);
	CoreAPIPtr shared_ptr;
	if (ptr != NULL) {
		shared_ptr = boost::static_pointer_cast<CoreAPI>(reinterpret_cast<CoreAPI *>(ptr)->shared_from_this());
	}
	return shared_ptr;
}

PayloadTypeAPIPtr FactoryAPI::getPayloadType(PayloadType *payloadType) {
	FBLOG_DEBUG("FactoryAPI::getPayloadType", "this=" << this << "\t" << "payloadType=" << payloadType);
	if (payloadType == NULL)
		return PayloadTypeAPIPtr();
	
	//void *ptr = payloadType->user_data;
	PayloadTypeAPIPtr shared_ptr;
	/*if (ptr == NULL) {*/
	shared_ptr = PayloadTypeAPIPtr(new PayloadTypeAPI(payloadType));
	/*} else {
	 shared_ptr = boost::static_pointer_cast<PayloadTypeAPI>(reinterpret_cast<PayloadTypeAPI *>(ptr)->shared_from_this());
	 }*/
	return get(shared_ptr, false, false);
}

PayloadTypeAPIPtr FactoryAPI::getPayloadType(const PayloadType *payloadType) {
	FBLOG_DEBUG("FactoryAPI::getPayloadType", "this=" << this << "\t" << "payloadType=" << payloadType);
	if (payloadType == NULL)
		return PayloadTypeAPIPtr();
	
	//void *ptr = payloadType->user_data;
	PayloadTypeAPIPtr shared_ptr;
	/*if (ptr == NULL) {*/
	shared_ptr = PayloadTypeAPIPtr(new PayloadTypeAPI(payloadType));
	/*} else {
	 shared_ptr = boost::static_pointer_cast<PayloadTypeAPI>(reinterpret_cast<PayloadTypeAPI *>(ptr)->shared_from_this());
	 }*/
	return get(shared_ptr, false, true);
}

ProxyConfigAPIPtr FactoryAPI::getProxyConfig(LinphoneProxyConfig *proxyConfig) {
	FBLOG_DEBUG("FactoryAPI::getProxyConfig", "this=" << this << "\t" << "proxyConfig=" << proxyConfig);
	if (proxyConfig == NULL)
		return ProxyConfigAPIPtr();
	
	void *ptr = linphone_proxy_config_get_user_data(proxyConfig);
	ProxyConfigAPIPtr shared_ptr;
	if (ptr == NULL) {
		shared_ptr = ProxyConfigAPIPtr(new ProxyConfigAPI(proxyConfig));
		get(shared_ptr, false, false);
	} else {
		shared_ptr = boost::static_pointer_cast<ProxyConfigAPI>(reinterpret_cast<ProxyConfigAPI *>(ptr)->shared_from_this());
	}
	return shared_ptr;
}
	
ProxyConfigAPIPtr FactoryAPI::getProxyConfig() {
	FBLOG_DEBUG("FactoryAPI::getProxyConfig", "this=" << this);
	ProxyConfigAPIPtr shared_ptr = ProxyConfigAPIPtr(new ProxyConfigAPI());
	get(shared_ptr, true, false);
	return shared_ptr;
}

SipTransportsAPIPtr FactoryAPI::getSipTransports() {
	FBLOG_DEBUG("FactoryAPI::getSipTransports", "this=" << this);
	SipTransportsAPIPtr shared_ptr = SipTransportsAPIPtr(new SipTransportsAPI());
	return get(shared_ptr, true, false);
}
	
VideoPolicyAPIPtr FactoryAPI::getVideoPolicy() {
	FBLOG_DEBUG("FactoryAPI::getVideoPolicy", "this=" << this);
	VideoPolicyAPIPtr shared_ptr = VideoPolicyAPIPtr(new VideoPolicyAPI());
	return get(shared_ptr, true, false);
}

FileManagerAPIPtr FactoryAPI::getFileManager() {
	FBLOG_DEBUG("FactoryAPI::getFileManager", "this=" << this);
	FileManagerAPIPtr shared_ptr = mFileManager.lock();
	if(!shared_ptr) {
		shared_ptr = FileManagerAPIPtr(new FileManagerAPI());
		get(shared_ptr, true, false);
		mFileManager = shared_ptr;
	}
	return shared_ptr;
}

FileTransferAPIPtr FactoryAPI::getFileTransfer(const FB::URI &sourceUri, const FB::URI &targetUri, const FB::JSObjectPtr &callback) {
	FBLOG_DEBUG("FactoryAPI::getFileTransfer", "this=" << this
				<< "\t" << "sourceUri=" << sourceUri.toString()
				<< "\t" << "targetUri=" << targetUri.toString()
				<< "\t" << "callback=" << callback);
	FileTransferAPIPtr shared_ptr;
	if(FileManagerAPI::isFile(sourceUri) && FileManagerAPI::isHttp(targetUri)) {
		shared_ptr = FileTransferAPIPtr(new UploadFileTransferAPI(sourceUri, targetUri, callback));
		get(shared_ptr, true, false);
	} else if(FileManagerAPI::isHttp(sourceUri) && FileManagerAPI::isFile(targetUri)) {
		shared_ptr = FileTransferAPIPtr(new DownloadFileTransferAPI(sourceUri, targetUri, callback));
		get(shared_ptr, true, false);
	} else if(FileManagerAPI::isFile(sourceUri) && FileManagerAPI::isFile(targetUri)) {
		shared_ptr = FileTransferAPIPtr(new LocalFileTransferAPI(sourceUri, targetUri, callback));
		get(shared_ptr, true, false);
	}
	return shared_ptr;
}

} // LinphoneWeb
