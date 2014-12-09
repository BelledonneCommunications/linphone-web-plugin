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

#ifndef H_CHATROOMAPI
#define H_CHATROOMAPI

#include <linphone/linphonecore.h>
#include "wrapperapi.h"

namespace LinphoneWeb {
	FB_FORWARD_PTR(AddressAPI)
	FB_FORWARD_PTR(ChatMessageAPI)
	FB_FORWARD_PTR(ChatRoomAPI)
	FB_FORWARD_PTR(ContentAPI)
	FB_FORWARD_PTR(CoreAPI)

	class ChatRoomAPI: public WrapperAPI {
		friend class FactoryAPI;

	private:
		LinphoneChatRoom *mChatRoom;

		ChatRoomAPI(LinphoneChatRoom *chatRoom);

	protected:
		virtual void initProxy();

	public:
		virtual ~ChatRoomAPI();

		void compose();
		CoreAPIPtr getCore() const;
		ChatMessageAPIPtr createFileTransferMessage(ContentAPIPtr const &content);
		ChatMessageAPIPtr createMessage(StringPtr const &message);
		ChatMessageAPIPtr createMessage2(StringPtr const &message, StringPtr const &externalBodyUrl, int state, time_t time, bool isRead, bool isIncoming);
		void deleteHistory();
		void deleteMessage(ChatMessageAPIPtr const &chatMessage);
		std::vector<ChatMessageAPIPtr> getHistoryRange(int begin, int end) const;
		int getHistorySize() const;
		AddressAPIPtr getPeerAddress() const;
		int getUnreadMessagesCount() const;
		void markAsRead();
		bool remoteComposing() const;
		void sendChatMessage(ChatMessageAPIPtr const &chatMessage);

		inline LinphoneChatRoom *getRef() {
			return mChatRoom;
		}
	};
} // LinphoneWeb
	
#endif //H_CHATROOMAPI