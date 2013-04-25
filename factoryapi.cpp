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

FactoryAPI::FactoryAPI(const CorePluginWeakPtr &plugin): mPlugin(plugin) {
    FBLOG_DEBUG("FactoryAPI::FactoryAPI()", "this=" << this);
}

FactoryAPI::~FactoryAPI() {
    FBLOG_DEBUG("FactoryAPI::~FactoryAPI()", "this=" << this);
}

CorePluginPtr FactoryAPI::getPlugin() {
	CorePluginPtr plugin(mPlugin.lock());
	if (!plugin) {
		throw FB::script_error("The plugin is invalid");
	}
	return plugin;
}

AddressAPIPtr FactoryAPI::get(LinphoneAddress *address) {
    FBLOG_DEBUG("FactoryAPI::get()", "this=" << this << "\t" << "address=" << address);
	if (address == NULL)
		return get(AddressAPIPtr());
    
	AddressAPIPtr shared_ptr;
	shared_ptr = AddressAPIPtr(new AddressAPI(address));
	return get(shared_ptr);
}

AddressAPIPtr FactoryAPI::get(const LinphoneAddress *address) {
    FBLOG_DEBUG("FactoryAPI::get()", "this=" << this << "\t" << "address=" << address);
	if (address == NULL)
		return get(AddressAPIPtr());
    
	AddressAPIPtr shared_ptr;
	shared_ptr = AddressAPIPtr(new AddressAPI(address));
	return get(shared_ptr);
}

AuthInfoAPIPtr FactoryAPI::get(LinphoneAuthInfo *authInfo) {
    FBLOG_DEBUG("FactoryAPI::get()", "this=" << this << "\t" << "authInfo=" << authInfo);
	if (authInfo == NULL)
		return get(AuthInfoAPIPtr());
    
	AuthInfoAPIPtr shared_ptr;
	shared_ptr = AuthInfoAPIPtr(new AuthInfoAPI(authInfo));
	return get(shared_ptr);
}

AuthInfoAPIPtr FactoryAPI::get(const LinphoneAuthInfo *authInfo) {
    FBLOG_DEBUG("FactoryAPI::get()", "this=" << this << "\t" << "authInfo=" << authInfo);
	if (authInfo == NULL)
		return get(AuthInfoAPIPtr());
    
	AuthInfoAPIPtr shared_ptr;
	shared_ptr = AuthInfoAPIPtr(new AuthInfoAPI(authInfo));
	return get(shared_ptr);
}

CallAPIPtr FactoryAPI::get(LinphoneCall *call) {
    FBLOG_DEBUG("FactoryAPI::get()", "this=" << this << "\t" << "call=" << call);
	if (call == NULL)
		return get(CallAPIPtr());
    
	void *ptr = linphone_call_get_user_pointer(call);
	CallAPIPtr shared_ptr;
	if (ptr == NULL) {
		shared_ptr = CallAPIPtr(new CallAPI(call));
	} else {
		shared_ptr = boost::static_pointer_cast<CallAPI>(reinterpret_cast<CallAPI *>(ptr)->shared_from_this());
	}
	return get(shared_ptr);
}

CallLogAPIPtr FactoryAPI::get(LinphoneCallLog *callLog) {
    FBLOG_DEBUG("FactoryAPI::get()", "this=" << this << "\t" << "callLog=" << callLog);
	if (callLog == NULL)
		return get(CallLogAPIPtr());
    
	void *ptr = linphone_call_log_get_user_pointer(callLog);
	CallLogAPIPtr shared_ptr;
	if (ptr == NULL) {
		shared_ptr = CallLogAPIPtr(new CallLogAPI(callLog));
	} else {
		shared_ptr = boost::static_pointer_cast<CallLogAPI>(reinterpret_cast<CallLogAPI *>(ptr)->shared_from_this());
	}
	return get(shared_ptr);
}

CallParamsAPIPtr FactoryAPI::get(LinphoneCallParams *callParams) {
    FBLOG_DEBUG("FactoryAPI::get()", "this=" << this << "\t" << "callParams=" << callParams);
	if (callParams == NULL)
		return get(CallParamsAPIPtr());
    
	CallParamsAPIPtr shared_ptr;
	shared_ptr = CallParamsAPIPtr(new CallParamsAPI(callParams));
	return get(shared_ptr);
}

CallParamsAPIPtr FactoryAPI::get(const LinphoneCallParams *callParams) {
    FBLOG_DEBUG("FactoryAPI::get()", "this=" << this << "\t" << "callParams=" << callParams);
	if (callParams == NULL)
		return get(CallParamsAPIPtr());
    
	CallParamsAPIPtr shared_ptr;
	shared_ptr = CallParamsAPIPtr(new CallParamsAPI(callParams));
	return get(shared_ptr);
}

CallStatsAPIPtr FactoryAPI::get(const LinphoneCallStats *callStats) {
    FBLOG_DEBUG("FactoryAPI::get()", "this=" << this << "\t" << "callStats=" << callStats);
	if (callStats == NULL)
		return get(CallStatsAPIPtr());
    
	CallStatsAPIPtr shared_ptr;
	shared_ptr = CallStatsAPIPtr(new CallStatsAPI(callStats));
	return get(shared_ptr);
}

CoreAPIPtr FactoryAPI::get(LinphoneCore *core) {
    FBLOG_DEBUG("FactoryAPI::get()", "this=" << this << "\t" << "core=" << core);
	if (core == NULL)
		return get(CoreAPIPtr(new CoreAPI()));
    
	void *ptr = linphone_core_get_user_data(core);
	CoreAPIPtr shared_ptr;
	if (ptr != NULL) {
		shared_ptr = boost::static_pointer_cast<CoreAPI>(reinterpret_cast<CoreAPI *>(ptr)->shared_from_this());
	}
	return get(shared_ptr);
}

PayloadTypeAPIPtr FactoryAPI::get(PayloadType *payloadType) {
    FBLOG_DEBUG("FactoryAPI::get()", "this=" << this << "\t" << "payloadType=" << payloadType);
	if (payloadType == NULL)
		return get(PayloadTypeAPIPtr());
    
	//void *ptr = payloadType->user_data;
	PayloadTypeAPIPtr shared_ptr;
	/*if (ptr == NULL) {*/
	shared_ptr = PayloadTypeAPIPtr(new PayloadTypeAPI(payloadType));
	/*} else {
	 shared_ptr = boost::static_pointer_cast<PayloadTypeAPI>(reinterpret_cast<PayloadTypeAPI *>(ptr)->shared_from_this());
	 }*/
	return get(shared_ptr);
}

PayloadTypeAPIPtr FactoryAPI::get(const PayloadType *payloadType) {
    FBLOG_DEBUG("FactoryAPI::get()", "this=" << this << "\t" << "payloadType=" << payloadType);
	if (payloadType == NULL)
		return get(PayloadTypeAPIPtr());
    
	//void *ptr = payloadType->user_data;
	PayloadTypeAPIPtr shared_ptr;
	/*if (ptr == NULL) {*/
	shared_ptr = PayloadTypeAPIPtr(new PayloadTypeAPI(payloadType));
	/*} else {
	 shared_ptr = boost::static_pointer_cast<PayloadTypeAPI>(reinterpret_cast<PayloadTypeAPI *>(ptr)->shared_from_this());
	 }*/
	return get(shared_ptr);
}


ProxyConfigAPIPtr FactoryAPI::get(LinphoneProxyConfig *proxyConfig) {
    FBLOG_DEBUG("FactoryAPI::get()", "this=" << this << "\t" << "proxyConfig=" << proxyConfig);
	if (proxyConfig == NULL)
		return get(ProxyConfigAPIPtr());
    
	void *ptr = linphone_proxy_config_get_user_data(proxyConfig);
	ProxyConfigAPIPtr shared_ptr;
	if (ptr == NULL) {
		shared_ptr = ProxyConfigAPIPtr(new ProxyConfigAPI(proxyConfig));
	} else {
		shared_ptr = boost::static_pointer_cast<ProxyConfigAPI>(reinterpret_cast<ProxyConfigAPI *>(ptr)->shared_from_this());
	}
	return get(shared_ptr);
}

FileManagerAPIPtr FactoryAPI::getFileManager() {
    if(!mFileManager) {
        mFileManager = get(FileManagerAPIPtr(new FileManagerAPI()));
    }
    return mFileManager;
}

FileTransferAPIPtr FactoryAPI::getFileTransfer(const FB::URI &sourceUri, const FB::URI &targetUri, const FB::JSObjectPtr &callback) {
    if(FileManagerAPI::isFile(sourceUri) && FileManagerAPI::isHttp(targetUri)) {
        return get(FileTransferAPIPtr(new UploadFileTransferAPI(sourceUri, targetUri, callback)));
    } else if(FileManagerAPI::isHttp(sourceUri) && FileManagerAPI::isFile(targetUri)) {
        return get(FileTransferAPIPtr(new DownloadFileTransferAPI(sourceUri, targetUri, callback)));
    } else if(FileManagerAPI::isFile(sourceUri) && FileManagerAPI::isFile(targetUri)) {
        return get(FileTransferAPIPtr(new LocalFileTransferAPI(sourceUri, targetUri, callback)));
    }
    return FileTransferAPIPtr();
}
