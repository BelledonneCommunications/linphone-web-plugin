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

#ifndef H_WRAPPERAPI
#define H_WRAPPERAPI

#include <JSAPIAuto.h>
#include "whiteboard.h"
#include "utils.h"
#include "macro.h"

namespace LinphoneWeb {

FB_FORWARD_PTR(FactoryAPI)

FB_FORWARD_PTR(WrapperAPI)
class WrapperAPI: public FB::JSAPIAuto {
	friend class FactoryAPI;
private:
	ThreadGroup mThreads;
	
	mutable FactoryAPIPtr mFactory;
	bool mOwned;
	bool mConst;
protected:
	virtual void setFactory(FactoryAPIPtr factory);
	virtual void initProxy() = 0;
	
	WrapperAPI(std::string const &description = "<JSAPI-Auto Javascript Object>");
	virtual ~WrapperAPI() = 0;
	
	// Thread
	void attachThread(boost::shared_ptr<boost::thread> const &thread);
	void detachThread(boost::thread::id id);
	
public:
	virtual FactoryAPIPtr getFactory() const;
	void disOwn();
	void own();
	bool isOwned() const;
	bool isConst() const;
	virtual void shutdown();
};

} // LinphoneWeb

#endif // H_WRAPPERAPI
