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

#ifndef H_CALLAPI
#define H_CALLAPI

#include <JSAPIAuto.h>
#include <linphonecore.h>

class CallAPI : public FB::JSAPIAuto {
private:
	LinphoneCall *mCall;
	CallAPI(LinphoneCall *call);

public:
	~CallAPI();
	std::string getRemoteAddress() const;
	int getDir() const;
	int getState() const;
	inline LinphoneCall *getRef() const{
		return mCall;
	}
	static boost::shared_ptr<CallAPI> get(LinphoneCall *call);
};

#endif //H_CALLAPI
