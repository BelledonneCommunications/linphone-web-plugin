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

#include "chatroomapi.h"
#include "contentapi.h"
#include "factoryapi.h"

namespace LinphoneWeb {

ChatRoomAPI::ChatRoomAPI(LinphoneChatRoom *chatRoom) :
		WrapperAPI(APIDescription(this)), mChatRoom(chatRoom) {
	FBLOG_DEBUG("ChatRoomAPI::ChatRoomAPI", "this=" << this << "\t" << "chatRoom=" << chatRoom);
	linphone_chat_room_ref(mChatRoom);
	linphone_chat_room_set_user_data(mChatRoom, this);
}

ChatRoomAPI::~ChatRoomAPI() {
	FBLOG_DEBUG("ChatRoomAPI::~ChatRoomAPI", "this=" << this);
	if (mChatRoom != NULL) {
		linphone_chat_room_set_user_data(mChatRoom, NULL);
		linphone_chat_room_unref(mChatRoom);
	}
}

void ChatRoomAPI::initProxy() {
	registerProperty("core", make_property(this, &ChatRoomAPI::getCore));
	registerProperty("historySize", make_property(this, &ChatRoomAPI::getHistorySize));
	registerProperty("peerAddress", make_property(this, &ChatRoomAPI::getPeerAddress));
	registerProperty("remoteComposing", make_property(this, &ChatRoomAPI::remoteComposing));
	registerProperty("unreadMessagesCount", make_property(this, &ChatRoomAPI::getUnreadMessagesCount));

	registerMethod("compose", make_method(this, &ChatRoomAPI::compose));
	registerMethod("deleteHistory", make_method(this, &ChatRoomAPI::deleteHistory));
	registerMethod("deleteMessage", make_method(this, &ChatRoomAPI::deleteMessage));
	registerMethod("getHistoryRange", make_method(this, &ChatRoomAPI::getHistoryRange));
	registerMethod("markAsRead", make_method(this, &ChatRoomAPI::markAsRead));
	registerMethod("newFileTransferMessage", make_method(this, &ChatRoomAPI::createFileTransferMessage));
	registerMethod("newMessage", make_method(this, &ChatRoomAPI::createMessage));
	registerMethod("newMessage2", make_method(this, &ChatRoomAPI::createMessage2));
	registerMethod("sendChatMessage", make_method(this, &ChatRoomAPI::sendChatMessage));
}

void ChatRoomAPI::compose() {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::compose", "this=" << this);
	linphone_chat_room_compose(mChatRoom);
}

void ChatRoomAPI::deleteHistory() {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::deleteHistory", "this=" << this);
	linphone_chat_room_delete_history(mChatRoom);
}

void ChatRoomAPI::deleteMessage(ChatMessageAPIPtr const &chatMessage) {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::deleteMessage", "this=" << this << "\t" << "chatMessage=" << chatMessage);
	linphone_chat_room_delete_message(mChatRoom, chatMessage->getRef());
}

CoreAPIPtr ChatRoomAPI::getCore() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::getCore", "this=" << this);
	return getFactory()->getCore(linphone_chat_room_get_core(mChatRoom));
}

ChatMessageAPIPtr ChatRoomAPI::createFileTransferMessage(ContentAPIPtr const &content) {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::createFileTransferMessage", "this=" << this << "\t" << "content=" << content);
	LinphoneChatMessage *chatMessage = linphone_chat_room_create_file_transfer_message(mChatRoom, content->getRef());
	return getFactory()->getChatMessage(chatMessage);
}

ChatMessageAPIPtr ChatRoomAPI::createMessage(StringPtr const &message) {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::createMessage", "this=" << this << "\t" << "message=" << message);
	LinphoneChatMessage *chatMessage = linphone_chat_room_create_message(mChatRoom, STRING_TO_CHARPTR(message));
	return getFactory()->getChatMessage(chatMessage);
}

ChatMessageAPIPtr ChatRoomAPI::createMessage2(StringPtr const &message, StringPtr const &externalBodyUrl, int state, time_t time, bool isRead, bool isIncoming) {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::createMessage2", "this=" << this << "\t" << "message=" << message << "\t" << "externalBodyUrl=" << externalBodyUrl << "\t" << "time=" << time << "\t" << "isIncoming=" << isIncoming);
	LinphoneChatMessage *chatMessage = linphone_chat_room_create_message_2(mChatRoom, STRING_TO_CHARPTR(message), STRING_TO_CHARPTR(externalBodyUrl), (LinphoneChatMessageState)state, time, isRead, isIncoming);
	return getFactory()->getChatMessage(chatMessage);
}

std::vector<ChatMessageAPIPtr> ChatRoomAPI::getHistoryRange(int begin, int end) const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::getHistoryRange", "this=" << this << "\t" << "begin=" << begin << "\t" << "end=" << end);
	std::vector<ChatMessageAPIPtr> list;
	for (const MSList *node = linphone_chat_room_get_history_range(mChatRoom, begin, end); node != NULL; node = ms_list_next(node)) {
		list.push_back(getFactory()->getChatMessage(reinterpret_cast<LinphoneChatMessage *>(node->data)));
	}
	return list;
}

int ChatRoomAPI::getHistorySize() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::getHistorySize", "this=" << this);
	return linphone_chat_room_get_history_size(mChatRoom);
}

AddressAPIPtr ChatRoomAPI::getPeerAddress() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::getPeerAddress", "this=" << this);
	const LinphoneAddress *peerAddress = linphone_chat_room_get_peer_address(mChatRoom);
	return getFactory()->getAddress(peerAddress);
}

int ChatRoomAPI::getUnreadMessagesCount() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::getUnreadMessagesCount", "this=" << this);
	return linphone_chat_room_get_unread_messages_count(mChatRoom);
}

void ChatRoomAPI::markAsRead() {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::markAsRead", "this=" << this);
	linphone_chat_room_mark_as_read(mChatRoom);
}

bool ChatRoomAPI::remoteComposing() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::remoteComposing", "this=" << this);
	return (linphone_chat_room_is_remote_composing(mChatRoom) == TRUE);
}

void ChatRoomAPI::sendChatMessage(ChatMessageAPIPtr const &chatMessage) {
	CORE_MUTEX
	FBLOG_DEBUG("ChatRoomAPI::sendChatMessage", "this=" << this << "\t" << "chatMessage=" << chatMessage);
	linphone_chat_room_send_chat_message(mChatRoom, chatMessage->getRef());
}

} // LinphoneWeb
