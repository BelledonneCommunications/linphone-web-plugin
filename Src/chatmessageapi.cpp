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

#include "chatmessageapi.h"
#include "factoryapi.h"

namespace LinphoneWeb {

ChatMessageAPI::ChatMessageAPI(LinphoneChatMessage *chatMessage) :
		WrapperAPI(APIDescription(this)), mChatMessage(chatMessage) {
	FBLOG_DEBUG("ChatMessageAPI::ChatMessageAPI", "this=" << this << "\t" << "chatMessage=" << chatMessage);
	linphone_chat_message_ref(mChatMessage);
	linphone_chat_message_set_user_data(mChatMessage, this);
}

ChatMessageAPI::~ChatMessageAPI() {
	FBLOG_DEBUG("ChatMessageAPI::~ChatMessageAPI", "this=" << this);
	if (mChatMessage != NULL) {
		linphone_chat_message_set_user_data(mChatMessage, NULL);
		linphone_chat_message_unref(mChatMessage);
	}
}

void ChatMessageAPI::initProxy() {
	registerProperty("appdata", make_property(this, &ChatMessageAPI::getAppdata, &ChatMessageAPI::setAppdata));
	registerProperty("chatRoom", make_property(this, &ChatMessageAPI::getChatRoom));
	registerProperty("errorInfo", make_property(this, &ChatMessageAPI::getErrorInfo));
	registerProperty("externalBodyUrl", make_property(this, &ChatMessageAPI::getExternalBodyUrl));
	registerProperty("fileTransferFilePath", make_property(this, &ChatMessageAPI::getFileTransferFilepath));
	registerProperty("fromAddress", make_property(this, &ChatMessageAPI::getFromAddress));
	registerProperty("localAddress", make_property(this, &ChatMessageAPI::getLocalAddress));
	registerProperty("outgoing", make_property(this, &ChatMessageAPI::outgoing));
	registerProperty("peerAddress", make_property(this, &ChatMessageAPI::getPeerAddress));
	registerProperty("read", make_property(this, &ChatMessageAPI::read));
	registerProperty("state", make_property(this, &ChatMessageAPI::getState));
	registerProperty("storageId", make_property(this, &ChatMessageAPI::getStorageId));
	registerProperty("text", make_property(this, &ChatMessageAPI::getText));
	registerProperty("time", make_property(this, &ChatMessageAPI::getTime));
	registerProperty("toAddress", make_property(this, &ChatMessageAPI::getToAddress));

	registerMethod("addCustomHeader", make_method(this, &ChatMessageAPI::addCustomHeader));
	registerMethod("cancelFileTransfer", make_method(this, &ChatMessageAPI::cancelFileTransfer));
	registerMethod("clone", make_method(this, &ChatMessageAPI::clone));
	registerMethod("getCustomHeader", make_method(this, &ChatMessageAPI::getCustomHeader));
}

void ChatMessageAPI::addCustomHeader(StringPtr const &headerName, StringPtr const &headerValue) {
	CORE_MUTEX
	FBLOG_DEBUG("ChatMessageAPI::addCustomHeader", "this=" << this << "\t" << "headerName=" << headerName << "\t" << "headerValue=" << headerValue);
	return linphone_chat_message_add_custom_header(mChatMessage, STRING_TO_CHARPTR(headerName), STRING_TO_CHARPTR(headerValue));
}

StringPtr ChatMessageAPI::getAppdata() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatMessageAPI::getAppdata", "this=" << this);
	return CHARPTR_TO_STRING(linphone_chat_message_get_appdata(mChatMessage));
}

void ChatMessageAPI::setAppdata(StringPtr const &data) {
	CORE_MUTEX
	FBLOG_DEBUG("ChatMessageAPI::setAppdata", "this=" << this << "\t" << "data=" << data);
	linphone_chat_message_set_appdata(mChatMessage, STRING_TO_CHARPTR(data));
}

void ChatMessageAPI::cancelFileTransfer() {
	CORE_MUTEX
	FBLOG_DEBUG("ChatMessageAPI::cancelFileTransfer", "this=" << this);
	linphone_chat_message_cancel_file_transfer(mChatMessage);
}

ChatRoomAPIPtr ChatMessageAPI::getChatRoom() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatMessageAPI::getChatRoom", "this=" << this);
	LinphoneChatRoom *chatRoom = linphone_chat_message_get_chat_room(mChatMessage);
	return getFactory()->getChatRoom(chatRoom);
}

ChatMessageAPIPtr ChatMessageAPI::clone() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatMessageAPI::clone", "this=" << this);
	LinphoneChatMessage *chatMessage = linphone_chat_message_clone(mChatMessage);
	return getFactory()->getChatMessage(chatMessage);
}

ErrorInfoAPIPtr ChatMessageAPI::getErrorInfo() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatMessageAPI::getErrorInfo", "this=" << this);
	const LinphoneErrorInfo *errorInfo = linphone_chat_message_get_error_info(mChatMessage);
	return getFactory()->getErrorInfo(errorInfo);
}

StringPtr ChatMessageAPI::getExternalBodyUrl() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatMessageAPI::getExternalBodyUrl", "this=" << this);
	return CHARPTR_TO_STRING(linphone_chat_message_get_external_body_url(mChatMessage));
}

StringPtr ChatMessageAPI::getFileTransferFilepath() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatMessageAPI::getFileTransferFilepath", "this=" << this);
	return CHARPTR_TO_STRING(linphone_chat_message_get_file_transfer_filepath(mChatMessage));
}

AddressAPIPtr ChatMessageAPI::getFromAddress() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatMessageAPI::getFromAddress", "this=" << this);
	const LinphoneAddress *fromAddress = linphone_chat_message_get_from_address(mChatMessage);
	return getFactory()->getAddress(fromAddress);
}

StringPtr ChatMessageAPI::getCustomHeader(StringPtr const &headerName) {
	CORE_MUTEX
	FBLOG_DEBUG("ChatMessageAPI::getCustomHeader", "this=" << this << "\t" << "headerName=" << headerName);
	return CHARPTR_TO_STRING(linphone_chat_message_get_custom_header(mChatMessage, STRING_TO_CHARPTR(headerName)));
}

AddressAPIPtr ChatMessageAPI::getLocalAddress() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatMessageAPI::getLocalAddress", "this=" << this);
	LinphoneAddress *localAddress = linphone_chat_message_get_local_address(mChatMessage);
	return getFactory()->getAddress(localAddress);
}

bool ChatMessageAPI::outgoing() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatMessageAPI::outgoing", "this=" << this);
	return (linphone_chat_message_is_outgoing(mChatMessage) == TRUE);
}

AddressAPIPtr ChatMessageAPI::getPeerAddress() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatMessageAPI::getPeerAddress", "this=" << this);
	const LinphoneAddress *peerAddress = linphone_chat_message_get_peer_address(mChatMessage);
	return getFactory()->getAddress(peerAddress);
}

bool ChatMessageAPI::read() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatMessageAPI::read", "this=" << this);
	return (linphone_chat_message_is_read(mChatMessage) == TRUE);
}

int ChatMessageAPI::getState() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatMessageAPI::getState", "this=" << this);
	return (int)linphone_chat_message_get_state(mChatMessage);
}

unsigned int ChatMessageAPI::getStorageId() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatMessageAPI::getStorageId", "this=" << this);
	return linphone_chat_message_get_storage_id(mChatMessage);
}

StringPtr ChatMessageAPI::getText() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatMessageAPI::getText", "this=" << this);
	return CHARPTR_TO_STRING(linphone_chat_message_get_text(mChatMessage));
}

time_t ChatMessageAPI::getTime() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatMessageAPI::getTime", "this=" << this);
	// WARNING: Keep only the least-significant 32 bits because of difference of definition of time_t between mingw and visual studio
	return (linphone_chat_message_get_time(mChatMessage) & 0xFFFFFFFF);
}

AddressAPIPtr ChatMessageAPI::getToAddress() const {
	CORE_MUTEX
	FBLOG_DEBUG("ChatMessageAPI::getToAddress", "this=" << this);
	const LinphoneAddress *toAddress = linphone_chat_message_get_to_address(mChatMessage);
	return getFactory()->getAddress(toAddress);
}

} // LinphoneWeb
