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

#include "wrapperapi.h"

namespace LinphoneWeb {

WrapperAPI::WrapperAPI(std::string const &description): FB::JSAPIAuto(description), mOwned(false), mConst(false) {
	
	// Reserved names
	setReserved("className");
	setReserved("classList");
	setReserved("parentNode");
}
	
WrapperAPI::~WrapperAPI() {
	// Ensure that all associated threads are ended
	// But if all the thread keeps reference to object this should never do something
	mThreads.joinAll();
}

void WrapperAPI::setFactory(FactoryAPIPtr factory) {
	mFactory = factory;
}

FactoryAPIPtr WrapperAPI::getFactory() const {
	return mFactory;
}
	
void WrapperAPI::disOwn() {
	mOwned = false;
}
	
void WrapperAPI::own() {
	mOwned = true;
}
	
bool WrapperAPI::isOwned() const {
	return mOwned;
}
	
bool WrapperAPI::isConst() const {
	return mConst;
}

void WrapperAPI::shutdown() {
	FBLOG_DEBUG("WrapperAPI::shutdown", "this=" << this);
	mThreads.interruptAll();
}

void WrapperAPI::attachThread(boost::shared_ptr<boost::thread> const &thread) {
	FBLOG_DEBUG("WrapperAPI::attachThread", "this=" << this  << "\t" << "id=" << thread->get_id());
	mThreads.addThread(thread);
}

void WrapperAPI::detachThread(boost::thread::id id) {
	FBLOG_DEBUG("WrapperAPI::detachThread", "this=" << this << "\t" << "id=" << id);
	mThreads.removeThread(id);
}

} // LinphoneWeb
