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

#ifndef H_CHATMESSAGEAPI
#define H_CHATMESSAGEAPI

#include <linphone/linphonecore.h>
#include "wrapperapi.h"

namespace LinphoneWeb {
	FB_FORWARD_PTR(AddressAPI)
	FB_FORWARD_PTR(ChatMessageAPI)
	FB_FORWARD_PTR(ChatRoomAPI)
	FB_FORWARD_PTR(ContentAPI)
	FB_FORWARD_PTR(ErrorInfoAPI)

	class ChatMessageAPI: public WrapperAPI {
		friend class FactoryAPI;

	private:
		LinphoneChatMessage *mChatMessage;

		ChatMessageAPI(LinphoneChatMessage *chatMessage);

	protected:
		virtual void initProxy();

	public:
		virtual ~ChatMessageAPI();

		DECLARE_PROPERTY_FILE(ChatMessageAPI, getFileTransferFilepath, setFileTransferFilepath);
		StringPtr getFileTransferFilepathOnFilesystem() const;
		void addCustomHeader(StringPtr const &headerName, StringPtr const &headerValue);
		StringPtr getAppdata() const;
		void setAppdata(StringPtr const &data);
		void cancelFileTransfer();
		ChatRoomAPIPtr getChatRoom() const;
		ChatMessageAPIPtr clone() const;
		ErrorInfoAPIPtr getErrorInfo() const;
		StringPtr getExternalBodyUrl() const;
		ContentAPIPtr getFileTransferInformation() const;
		AddressAPIPtr getFromAddress() const;
		StringPtr getCustomHeader(StringPtr const &headerName);
		AddressAPIPtr getLocalAddress() const;
		bool outgoing() const;
		AddressAPIPtr getPeerAddress() const;
		bool read() const;
		int getState() const;
		unsigned int getStorageId() const;
		StringPtr getText() const;
		time_t getTime() const;
		AddressAPIPtr getToAddress() const;
		void downloadFile();

		inline LinphoneChatMessage *getRef() {
			return mChatMessage;
		}

	public: // Event helpers
		FB_JSAPI_EVENT(msgStateChanged, 2, (ChatMessageAPIPtr, const int&));
		FB_JSAPI_EVENT(fileTransferProgressIndication, 4, (ChatMessageAPIPtr, ContentAPIPtr, size_t, size_t));

	protected:
		virtual void onMsgStateChanged(LinphoneChatMessageState state);
		virtual void onFileTransferProgressIndication(const LinphoneContent *content, size_t offset, size_t total);

	private:
		// C Wrappers
		static void wrapper_msg_state_changed(LinphoneChatMessage *msg, LinphoneChatMessageState state);
		static void wrapper_file_transfer_progress_indication(LinphoneChatMessage *message, const LinphoneContent* content, size_t offset, size_t total);
	};
} // LinphoneWeb
	
#endif //H_CHATMESSAGEAPI