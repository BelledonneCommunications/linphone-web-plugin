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

#include "whiteboard.h"

namespace LinphoneWeb {
	
	
const WhiteBoard::IdType WhiteBoard::NoId = 0;
const WhiteBoard::ValueType WhiteBoard::NoValue = WhiteBoard::ValueType();
	
WhiteBoard::WhiteBoard(): mNextId(1) {
	
}
	
WhiteBoard::IdType WhiteBoard::addValue(WhiteBoard::ValueType const &value) {
	FBLOG_DEBUG("WhiteBoard::addValue", "this=" << this << "\t" << "value=" << value);
	boost::shared_lock<boost::shared_mutex> guard(mMutex);
	
	MapType::right_const_iterator it = mMap.right.find(value);
	if(it != mMap.right.end()) {
		it->first.incRef();
		return it->second;
	}
	IdType id = mNextId++;
	mMap.insert(EntryType(id, ContainerType(value)));
	return id;
}

WhiteBoard::IdType WhiteBoard::getId(WhiteBoard::ValueType const &value, bool append) {
	FBLOG_DEBUG("WhiteBoard::getId", "this=" << this << "\t" << "value=" << value << "\t" << "append=" << append);
	
	boost::shared_lock<boost::shared_mutex> guard(mMutex);
	
	MapType::right_const_iterator it = mMap.right.find(value);
	if(it != mMap.right.end()) {
		return it->second;
	}
	if(append) {
		IdType id = mNextId++;
		mMap.insert(EntryType(id, ContainerType(value)));
		return id;
	}
	return WhiteBoard::NoId;
}

WhiteBoard::ValueType WhiteBoard::getVariantValue(WhiteBoard::IdType const &id) {
	boost::shared_lock<boost::shared_mutex> guard(mMutex);
	
	MapType::left_const_iterator it = mMap.left.find(id);
	if(it != mMap.left.end()) {
		return it->second.value();
	}
	return WhiteBoard::NoValue;
}

bool WhiteBoard::removeId(WhiteBoard::IdType const &id) {
	FBLOG_DEBUG("WhiteBoard::removeId", "this=" << this << "\t" << "id=" << id);
	boost::shared_lock<boost::shared_mutex> guard(mMutex);
	
	MapType::left_const_iterator it = mMap.left.find(id);
	if(it != mMap.left.end()) {
		bool toErase = it->second.decRef();
		if(toErase) {
			mMap.erase(EntryType(it->first, it->second));
		}
		return true;
	}
	return false;
}

bool WhiteBoard::removeValue(WhiteBoard::ValueType const &value) {
	FBLOG_DEBUG("WhiteBoard::removeId", "this=" << this << "\t" << "value=" << value);
	boost::shared_lock<boost::shared_mutex> guard(mMutex);
	
	MapType::right_const_iterator it = mMap.right.find(value);
	if(it != mMap.right.end()) {
		bool toErase = it->first.decRef();
		if(toErase) {
			mMap.erase(EntryType(it->second, it->first));
		}
		return true;
	}
	return false;
}
	
}