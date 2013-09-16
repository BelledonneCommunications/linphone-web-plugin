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

#ifndef H_FACTORYAPI
#define H_FACTORYAPI

#include <JSAPIAuto.h>
#include <FactoryBase.h>
#include <URI.h>
#include <linphone/linphonecore.h>
#include <linphone/linphonefriend.h>
#include <linphone/lpconfig.h>

#include "addressapi.h"
#include "authinfoapi.h"
#include "callapi.h"
#include "calllogapi.h"
#include "callparamsapi.h"
#include "callstatsapi.h"
#include "coreapi.h"
#include "friendapi.h"
#include "lpconfigapi.h"
#include "payloadtypeapi.h"
#include "proxyconfigapi.h"
#include "siptransportsapi.h"
#include "videoapi.h"
#include "videopolicyapi.h"

#include "whiteboard.h"
#include "filemanagerapi.h"
#include "Transfers/filetransferapi.h"

namespace LinphoneWeb {

FB_FORWARD_PTR(CorePlugin)

FB_FORWARD_PTR(FactoryAPI)

class FactoryAPI: public boost::enable_shared_from_this<FactoryAPI> {
private:
	FB::PluginCoreWeakPtr mPlugin;
	WhiteBoardPtr mWhiteBoard;
	FileManagerAPIWeakPtr mFileManager;
	template<typename TypePtr>
	TypePtr handle(TypePtr ptr, bool owned, bool constant) {
		ptr->mOwned = owned;
		ptr->mConst = constant;
		ptr->setFactory(shared_from_this());
		ptr->initProxy();
		return ptr;
	}
	mutable boost::recursive_mutex mCoreMutex;

public:
	FactoryAPI(FB::PluginCorePtr const &plugin, WhiteBoardPtr const &whiteboard);
	~FactoryAPI();

	FB::PluginCorePtr getPlugin() const;
	WhiteBoardPtr getWhiteBoard() const;
	boost::recursive_mutex &getCoreMutex() const;

	// Address
	AddressAPIPtr getAddress(LinphoneAddress *address);
	AddressAPIPtr getAddress(const LinphoneAddress *address);
	AddressAPIPtr getAddress(StringPtr const &address);

	// AuthInfo
	AuthInfoAPIPtr getAuthInfo(LinphoneAuthInfo *authInfo);
	AuthInfoAPIPtr getAuthInfo(const LinphoneAuthInfo *authInfo);
	AuthInfoAPIPtr getAuthInfo(StringPtr const &username, StringPtr const &userid,
				StringPtr const &passwd, StringPtr const &ha1, StringPtr const &realm);

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

	// Friend
	FriendAPIPtr getFriend(LinphoneFriend *f);

	// LpConfig
	LpConfigAPIPtr getLpConfig(LpConfig *config);
	LpConfigAPIPtr getLpConfig(StringPtr const &configFilename);

	// PayloadType
	PayloadTypeAPIPtr getPayloadType(PayloadType *payloadType);
	PayloadTypeAPIPtr getPayloadType(const PayloadType *payloadType);

	// ProxyConfig
	ProxyConfigAPIPtr getProxyConfig(LinphoneProxyConfig *proxyConfig);
	ProxyConfigAPIPtr getProxyConfig();

	// SipTransports
	SipTransportsAPIPtr getSipTransports();

	// Video getVideo
	VideoAPIPtr getVideo();

	// VideoPolicy
	VideoPolicyAPIPtr getVideoPolicy();

	// File
	FileManagerAPIPtr getFileManager();
	FileTransferAPIPtr getFileTransfer(FB::URI const &sourceUri, FB::URI const &targetUri, FB::JSObjectPtr const &callback);
};

} // LinphoneWeb

#endif // H_FACTORYAPI
