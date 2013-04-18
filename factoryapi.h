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
#include <FBPointers.h>

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

FB_FORWARD_PTR(CorePlugin)

FB_FORWARD_PTR(FactoryAPI)
class FactoryAPI: public boost::enable_shared_from_this<FactoryAPI> {
private:
	CorePluginWeakPtr mPlugin;
    
    template<typename TypePtr>
    TypePtr get(TypePtr ptr) {
        ptr->mFactory = shared_from_this();
        return ptr;
    }
    
public:
    FactoryAPI(const CorePluginWeakPtr &plugin);
	CorePluginPtr getPlugin();
    
    // Address
    AddressAPIPtr get(LinphoneAddress *address);
	AddressAPIPtr get(const LinphoneAddress *address);
    
    // AuthInfo
    AuthInfoAPIPtr get(LinphoneAuthInfo *authInfo);
	AuthInfoAPIPtr get(const LinphoneAuthInfo *authInfo);
    
    // Call
    CallAPIPtr get(LinphoneCall *call);
    
    // CallLog
    CallLogAPIPtr get(LinphoneCallLog *callLog);
    
    // CallParams
    CallParamsAPIPtr get(LinphoneCallParams *callParams);
	CallParamsAPIPtr get(const LinphoneCallParams *callParams);
    
    // CallStats
	CallStatsAPIPtr get(const LinphoneCallStats *callStats);
    
    // Core
	CoreAPIPtr get(LinphoneCore *core);
    
    // PayloadType
    PayloadTypeAPIPtr get(PayloadType *payloadType);
	PayloadTypeAPIPtr get(const PayloadType *payloadType);
    
    // ProxyConfig
    ProxyConfigAPIPtr get(LinphoneProxyConfig *proxyConfig);
};

#endif // H_FACTORYAPI