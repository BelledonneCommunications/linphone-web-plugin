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

#include "wrapperapi.h"

WrapperAPI::WrapperAPI(const std::string& description): FB::JSAPIAuto(description) {
    
}

void WrapperAPI::setFactory(FactoryAPIPtr factory) {
    mFactory = factory;
}

void WrapperAPI::shutdown() {
    FBLOG_DEBUG("WrapperAPI::shutdown()", "this=" << this);
    mThreads.interruptAll();
}

void WrapperAPI::attachThread(const boost::shared_ptr<boost::thread> &thread) {
    FBLOG_DEBUG("WrapperAPI::attachThread()", "this=" << this  << "\t" << "id=" << thread->get_id());
    mThreads.addThread(thread);
}

void WrapperAPI::detachThread(boost::thread::id id) {
    FBLOG_DEBUG("WrapperAPI::detachThread()", "this=" << this << "\t" << "id=" << id);
    mThreads.removeThread(id);
}