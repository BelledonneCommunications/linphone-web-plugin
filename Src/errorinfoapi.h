/*!
 Linphone Web - Web plugin of Linphone an audio/video SIP phone
 Copyright (C) 2012-2014 Belledonne Communications

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

#ifndef H_ERRORINFOAPI
#define H_ERRORINFOAPI

#include <linphone/linphonecore.h>
#include "wrapperapi.h"

namespace LinphoneWeb {

FB_FORWARD_PTR(ErrorInfoAPI)
class ErrorInfoAPI: public WrapperAPI {
	friend class FactoryAPI;
private:
	int mReason;
	int mProtocolCode;
	StringPtr mPhrase;
	StringPtr mDetails;

	ErrorInfoAPI(const LinphoneErrorInfo *ei);

protected:
	virtual void initProxy();

public:
	virtual ~ErrorInfoAPI();

	int getReason() const;
	int getProtocolCode() const;
	StringPtr getPhrase() const;
	StringPtr getDetails() const;
};
	
} // LinphoneWeb

#endif //H_ERRORINFOAPI
