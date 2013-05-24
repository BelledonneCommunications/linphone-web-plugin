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

#ifndef H_FACTORYAPI
#define H_FACTORYAPI

#include <JSAPIAuto.h>
#include <URI.h>
#include <linphonecore.h>

#include "addressapi.h"
#include "authinfoapi.h"
#include "callapi.h"
#include "calllogapi.h"
#include "callparamsapi.h"
#include "callstatsapi.h"
#include "coreapi.h"
#include "payloadtypeapi.h"
#include "proxyconfigapi.h"
#include "siptransportsapi.h"

#include "filemanagerapi.h"
#include "Transfers/filetransferapi.h"

namespace LinphoneWeb {

FB_FORWARD_PTR(CorePlugin)

FB_FORWARD_PTR(FactoryAPI)
class FactoryAPI: public boost::enable_shared_from_this<FactoryAPI> {
private:
	CorePluginWeakPtr mPlugin;
	FileManagerAPIWeakPtr mFileManager;
	template<typename TypePtr>
	TypePtr get(TypePtr ptr) {
		ptr->setFactory(shared_from_this());
		return ptr;
	}
#ifdef CORE_THREADED
	mutable boost::mutex mCoreMutex;
#endif
public:
	FactoryAPI(const CorePluginWeakPtr &plugin);
	~FactoryAPI();
	
	CorePluginPtr getPlugin();
	boost::mutex &getCoreMutex();
	
	// Address
	AddressAPIPtr getAddress(LinphoneAddress *address);
	AddressAPIPtr getAddress(const LinphoneAddress *address);
	
	// AuthInfo
	AuthInfoAPIPtr getAuthInfo(LinphoneAuthInfo *authInfo);
	AuthInfoAPIPtr getAuthInfo(const LinphoneAuthInfo *authInfo);
	
	// Call
	CallAPIPtr getCall(LinphoneCall *call);
	
	// CallLog
	CallLogAPIPtr getCallLog(LinphoneCallLog *callLog);
	
	// CallParams
	CallParamsAPIPtr getCallParams(LinphoneCallParams *callParams);
	CallParamsAPIPtr getCallParams(const LinphoneCallParams *callParams);
	
	// CallStats
	CallStatsAPIPtr getCallStats(const LinphoneCallStats *callStats);
	
	// Core
	CoreAPIPtr getCore(LinphoneCore *core);
	
	// PayloadType
	PayloadTypeAPIPtr getPayloadType(PayloadType *payloadType);
	PayloadTypeAPIPtr getPayloadType(const PayloadType *payloadType);
	
	// ProxyConfig
	ProxyConfigAPIPtr getProxyConfig(LinphoneProxyConfig *proxyConfig);
	
	// SipTransports
	SipTransportsAPIPtr getSipTransports();
	
	// File
	FileManagerAPIPtr getFileManager();
	FileTransferAPIPtr getFileTransfer(const FB::URI &sourceUri, const FB::URI &targetUri, const FB::JSObjectPtr &callback);
};

} // LinphoneWeb

#endif // H_FACTORYAPI
