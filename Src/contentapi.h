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


 Authors:
 - Ghislain MARY <ghislain.mary@belledonne-communications.com>

 */

#ifndef H_CONTENTAPI
#define H_CONTENTAPI

#include <linphone/linphonecore.h>
#include "wrapperapi.h"

namespace LinphoneWeb {
	class ContentAPI: public WrapperAPI {
		friend class FactoryAPI;

	private:
		LinphoneContent *mContent;

		ContentAPI(LinphoneContent *content);
		ContentAPI(const LinphoneContent *content);

	protected:
		virtual void initProxy();

	public:
		virtual ~ContentAPI();

		StringPtr getBuffer() const;
		void setBuffer(StringPtr const &b64String);
		StringPtr getEncoding() const;
		void setEncoding(StringPtr const &encoding);
		StringPtr getName() const;
		void setName(StringPtr const &name);
		size_t getSize() const;
		void setSize(size_t size);
		StringPtr getStringBuffer() const;
		void setStringBuffer(StringPtr const &buffer);
		StringPtr getSubtype() const;
		void setSubtype(StringPtr const &subtype);
		StringPtr getType() const;
		void setType(StringPtr const &type);

		inline LinphoneContent *getRef() {
			return mContent;
		}
	};
} // LinphoneWeb
	
#endif //H_CONTENTAPI