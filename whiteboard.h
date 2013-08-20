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

#ifndef H_WHITEBOARD
#define H_WHITEBOARD

#include <JSAPIAuto.h>
#include <boost/bimap.hpp>
#include <boost/variant.hpp>
#include "wrapperapi.h"

namespace LinphoneWeb {
	
FB_FORWARD_PTR(WhiteBoard)
FB_FORWARD_PTR(VideoAPI)
	
class WhiteBoard {
public:
	typedef unsigned int IdType;
	typedef boost::variant<VideoAPIWeakPtr> ValueType;
	static const IdType NoId;
	static const ValueType NoValue;
	
private:
	
	template<typename U, typename T>
	class Container {
	private:
		mutable U mRef;
		T mValue;
		
	public:
		Container(T const &value): mRef(1), mValue(value) {
		}
		
		void incRef() const {
			mRef++;
		}
		bool decRef() const {
			return (--mRef) <= 0;
		}
		
		T const &value() const {
			return mValue;
		}
		
		
		bool operator== (const Container<U, T>& other) const {
			return mValue == other.mValue;
		}
		bool operator< (const Container<U, T>& other) const {
			return mValue < other.mValue;
		}
	};
	
	typedef Container< int, ValueType > ContainerType;
	typedef boost::bimap< IdType, ContainerType > MapType;
    typedef MapType::value_type EntryType;
	
	MapType mMap;
	unsigned int mNextId;
	mutable boost::shared_mutex mMutex;

private:
	ValueType getVariantValue(IdType const &id);
	
public:
	WhiteBoard();
	IdType addValue(ValueType const &value);
	IdType getId(ValueType const &value, bool append = false);
	template <typename T>
	T getValue(IdType const &id) {
		FBLOG_DEBUG("WhiteBoard::getValue", this << "\t" << "(Type)" << typeid(T).name() << "\t" << "id=" << id);
		ValueType variant = getVariantValue(id);
		T * value = boost::get<T>(&variant);
		if(!value) {
			throw FB::script_error("Invalid id");
		}
		return *value;
	}
	bool removeId(IdType const &id);
	bool removeValue(ValueType const &value);
};

}

#endif