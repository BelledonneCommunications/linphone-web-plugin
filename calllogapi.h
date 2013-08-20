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

#ifndef H_CALLLOGAPI
#define H_CALLLOGAPI

#include <linphone/linphonecore.h>
#include "wrapperapi.h"

namespace LinphoneWeb {

FB_FORWARD_PTR(AddressAPI)

FB_FORWARD_PTR(CallLogAPI)
class CallLogAPI: public WrapperAPI {
	friend class FactoryAPI;
private:
	LinphoneCallLog *mCallLog;

	CallLogAPI(LinphoneCallLog *callLog);
	
protected:
	virtual void initProxy();
	
public:
	virtual ~CallLogAPI();
	
	StringPtr getCallId() const;
	LinphoneCallDir getDir() const;
	int getDuration() const;
	AddressAPIPtr getFrom() const;
	float getQuality() const;
	AddressAPIPtr getRemoteAddress() const;
	time_t getStartDate() const;
	LinphoneCallStatus getStatus() const;
	AddressAPIPtr getTo() const;
	bool videoEnabled() const;
	
	StringPtr toStr() const;

	inline LinphoneCallLog *getRef() {
		return mCallLog;
	}
};
	
} // LinphoneWeb

#endif //H_CALLLOGAPI
