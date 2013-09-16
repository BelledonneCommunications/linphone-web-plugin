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
 - Ghislain MARY <ghislain.mary@belledonne-communications.com>

 */

#ifndef H_FRIENDAPI
#define H_FRIENDAPI

#include <linphone/linphonecore.h>
#include <linphone/linphonefriend.h>
#include "wrapperapi.h"

namespace LinphoneWeb {

FB_FORWARD_PTR(AddressAPI)
FB_FORWARD_PTR(FriendAPI)
FB_FORWARD_PTR(PresenceModelAPI)

class FriendAPI: public WrapperAPI {
	friend class FactoryAPI;

private:
	LinphoneFriend *mFriend;

	FriendAPI();
	FriendAPI(LinphoneFriend *lFriend);
	FriendAPI(StringPtr const &address);

protected:
	virtual void initProxy();

public:
	virtual ~FriendAPI();

	void setAddress(AddressAPIPtr const &address);
	AddressAPIPtr getAddress() const;
	void setName(StringPtr const &name);
	StringPtr getName() const;
	void enableSubscribes(bool enable);
	bool subscribesEnabled() const;
	void setIncSubscribePolicy(int policy);
	int getIncSubscribePolicy() const;
	void setRefKey(StringPtr const &key);
	StringPtr getRefKey() const;
	void edit();
	void done();
	bool inList() const;
	PresenceModelAPIPtr getPresenceModel() const;

	inline LinphoneFriend *getRef() {
		return mFriend;
	}
};

} // LinphoneWeb
	
#endif //H_FRIENDAPI
