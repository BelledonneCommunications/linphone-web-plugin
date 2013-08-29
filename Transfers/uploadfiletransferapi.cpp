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

#include "uploadfiletransferapi.h"
#include "../Src/coreplugin.h"
#include "../Src/factoryapi.h"
#include "../Src/base64.h"

namespace LinphoneWeb {

const unsigned int UploadFileTransferAPI::BUFFER_SIZE = 32 * 1024;

UploadFileTransferAPI::UploadFileTransferAPI(FB::URI const &sourceUri, FB::URI const &targetUri, FB::JSObjectPtr const &callback):
	FileTransferAPI(sourceUri, targetUri, callback) {
	FBLOG_DEBUG("UploadFileTransferAPI::UploadFileTransferAPI", "this=" << this);
}

UploadFileTransferAPI::~UploadFileTransferAPI() {
	FBLOG_DEBUG("UploadFileTransferAPI::~UploadFileTransferAPI", "this=" << this);
}

void UploadFileTransferAPI::start() {
	FBLOG_DEBUG("UploadFileTransferAPI::start", "this=" << this);
	mFileStr = getFactory()->getFileManager()->uriToFile(mSourceUri);
	if(mFileStr.empty()) {
		FBLOG_DEBUG("UploadFileTransferAPI::start", "Invalid source path");
		onError("Invalid source path");
		return;
	}
	FBLOG_DEBUG("UploadFileTransferAPI::start", "sourceFile=" << mFileStr);
	
	mFilePath = boost::filesystem::path(mFileStr);
	if(!boost::filesystem::exists(mFilePath)) {
		FBLOG_DEBUG("UploadFileTransferAPI::start", "The path source \"" << mFileStr << "\" doesn't exist");
		onError("The source path doesn't exist");
		return;
	}
	if(boost::filesystem::is_directory(mFilePath)) {
		FBLOG_DEBUG("UploadFileTransferAPI::start", "Can't upload a directory");
		onError("Can't transfer a directory");
		return;
	}
	
	mFileStream.open(mFileStr.c_str(), std::ios_base::in | std::ios_base::binary);
	if(mFileStream.fail()) {
		FBLOG_DEBUG("UploadFileTransferAPI::start", "Can't open the source file: " << mFileStr);
		onError("Can't open the source file");
		return;
	}
	
	// Run thread
	mThread = boost::make_shared<boost::thread>(boost::bind(&UploadFileTransferAPI::threadFctHolder, boost::static_pointer_cast<UploadFileTransferAPI>(shared_from_this())));
	attachThread(mThread);
}

void UploadFileTransferAPI::threadFctHolder(UploadFileTransferAPIPtr &self) {
	self->threadFct();
}


void UploadFileTransferAPI::threadFct() {
	FBLOG_DEBUG("UploadFileTransferAPI::threadFct", "this=" << this);
	try {
		// Read the file by chunk
		char buffer[BUFFER_SIZE];
		size_t readSize = 1;
		std::string binaryData;
		while(!boost::this_thread::interruption_requested() && readSize > 0) {
			readSize = (size_t)mFileStream.readsome(buffer, BUFFER_SIZE);
			if(mFileStream.fail()) {
				mFileStream.close();
				FBLOG_DEBUG("UploadFileTransferAPI::threadFct", "File read error");
				onError("File read error");
				throw std::runtime_error("File read error");
			}
			binaryData.append(buffer, readSize);
		}
		mFileStream.close();
		
		// Convert
		std::string base64Data = base64_encode(binaryData);
		
		// Create the POST data
		std::string postData;
		postData += "filename=";
		postData += mFilePath.filename().string();
		postData += "&data=";
		postData += base64Data;
		
		FBLOG_DEBUG("UploadFileTransferAPI::threadFct", mFileStr << "(" << binaryData.length() << "->" << base64Data.length() <<")");
		
		// Create the request
		FB::BrowserStreamRequest req(mTargetUri, "POST");
		req.setPostData(postData);
		req.setCallback(boost::bind(&UploadFileTransferAPI::callbackFct, this, _1, _2, _3, _4));
		
		// Start transfer
		FB::BrowserHostPtr host = getFactory()->getPlugin()->getHost();
		try {
			mHelper = FileStreamHelper::AsyncRequest(host, req);
		} catch(std::runtime_error&) {
			FBLOG_DEBUG("UploadFileTransferAPI::threadFct", "Internal error");
			onError("Internal error");
		}
	} catch(std::runtime_error&) {
		// Nothing to do
	}
	detachThread(boost::this_thread::get_id());
	mThread.reset();
}

void UploadFileTransferAPI::callbackFct(bool success, FB::HeaderMap const &headers, boost::shared_array<uint8_t> const &data, const size_t size) {
	FBLOG_DEBUG("UploadFileTransferAPI::callbackFct", "this=" << this);
	if(!success) {
		FBLOG_DEBUG("UploadFileTransferAPI::callbackFct", "HTTP error");
		onError("HTTP error");
		return;
	}
	onSuccess(success);
}

void UploadFileTransferAPI::cancel() {
	FBLOG_DEBUG("UploadFileTransferAPI::cancel", "this=" << this);
	if(mHelper) {
		mHelper->cancel();
	}
	if(mThread) {
		mThread->interrupt();
	}
}

long long UploadFileTransferAPI::getTransferedBytes() {
	FBLOG_DEBUG("UploadFileTransferAPI::getTransferedBytes", "this=" << this);
	if(mHelper) {
		mHelper->getReceived();
	}
	return -1;
}

long long UploadFileTransferAPI::getTotalBytes() {
	FBLOG_DEBUG("UploadFileTransferAPI::getTotalBytes", "this=" << this);
	if(mHelper) {
		mHelper->getStream()->getLength();
	}
	return -1;
}
	
} // LinphoneWeb
