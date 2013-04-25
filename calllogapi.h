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

#ifndef H_CALLLOGAPI
#define H_CALLLOGAPI

#include <linphonecore.h>
#include "wrapperapi.h"

FB_FORWARD_PTR(CallLogAPI)
class CallLogAPI: public WrapperAPI {
	friend class FactoryAPI;
private:
	LinphoneCallLog *mCallLog;

	CallLogAPI(LinphoneCallLog *callLog);
	
protected:
	void initProxy();
	
public:
	~CallLogAPI();

	std::string getRefKey() const;
	void setRefKey(const std::string &refKey);

	std::string toStr() const;

	inline LinphoneCallLog *getRef() {
		mUsed = true;
		return mCallLog;
	}
};

#endif //H_CALLLOGAPI
