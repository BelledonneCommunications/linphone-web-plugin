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

#include "downloadfiletransferapi.h"
#include "../Src/coreplugin.h"
#include "../Src/factoryapi.h"

namespace LinphoneWeb {

const unsigned int DownloadFileTransferAPI::BUFFER_SIZE = 32 * 1024;

DownloadFileTransferAPI::DownloadFileTransferAPI(FB::URI const &sourceUri, FB::URI const &targetUri, FB::JSObjectPtr const &callback):
	FileTransferAPI(sourceUri, targetUri, callback) {
	FBLOG_DEBUG("DownloadFileTransferAPI::DownloadFileTransferAPI", "this=" << this);
}

DownloadFileTransferAPI::~DownloadFileTransferAPI() {
	FBLOG_DEBUG("DownloadFileTransferAPI::~DownloadFileTransferAPI", "this=" << this);
}

void DownloadFileTransferAPI::start() {
	FBLOG_DEBUG("DownloadFileTransferAPI::start", "this=" << this);
	mFileStr = getFactory()->getFileManager()->uriToFile(mTargetUri);
	if(mFileStr.empty()) {
		FBLOG_DEBUG("DownloadFileTransferAPI::start", "Invalid target path");
		onError("Invalid target path");
		return;
	}
	FBLOG_DEBUG("DownloadFileTransferAPI::start", "targetFile=" << mFileStr);
	
	mFilePath = boost::filesystem::path(mFileStr);
	if(boost::filesystem::exists(mFilePath)) {
		FBLOG_DEBUG("DownloadFileTransferAPI::start", "The path target \"" << mFileStr << "\" already exists");
		onError("The target path already exist");
		return;
	}
	
	// Create the request
	FB::BrowserStreamRequest req(mSourceUri, "GET");
	req.setCallback(boost::bind(&DownloadFileTransferAPI::callbackFct, this, _1, _2, _3, _4));
	
	// Start transfer
	FB::BrowserHostPtr host = getFactory()->getPlugin()->getHost();
	try {
		mHelper = FileStreamHelper::AsyncRequest(host, req);
	} catch(std::runtime_error&) {
		FBLOG_DEBUG("DownloadFileTransferAPI::start", "Internal error");
		onError("Internal error");
	}
}

void DownloadFileTransferAPI::callbackFct(bool success, FB::HeaderMap const &headers, boost::shared_array<uint8_t> const &data, const size_t size) {
	FBLOG_DEBUG("UploadFileTransferAPI::callbackFct", "this=" << this);
	if(!success) {
		mFileStream.close();
		FBLOG_DEBUG("UploadFileTransferAPI::callbackFct", "HTTP error");
		onError("HTTP error");
		return;
	}
	
	// Run thread
	mThread = boost::make_shared<boost::thread>(boost::bind(DownloadFileTransferAPI::threadFctHolder, boost::static_pointer_cast<DownloadFileTransferAPI>(shared_from_this()), data, size));
	attachThread(mThread);
}

void DownloadFileTransferAPI::threadFctHolder(DownloadFileTransferAPIPtr &self, boost::shared_array<uint8_t> const &adata, size_t &size) {
	self->threadFct(adata, size);
}

void DownloadFileTransferAPI::threadFct(boost::shared_array<uint8_t> const &adata, size_t &size) {
	FBLOG_DEBUG("DownloadFileTransferAPI::threadFct", "this=" << this);
	
	mFileStream.open(mFileStr.c_str(), std::ios_base::out | std::ios_base::binary);
	if(mFileStream.fail()) {
		FBLOG_DEBUG("UploadFileTransferAPI::start", "Can't open the target file: " << mFileStr);
		onError("Can't open the target file");
		return;
	}
	
	try {
		// Write the file by chunck
		const char *data = (const char *)adata.get();
		size_t writeSize;
		while(!boost::this_thread::interruption_requested() && size > 0) {
			writeSize = size;
			if(writeSize > BUFFER_SIZE) writeSize = BUFFER_SIZE;
			mFileStream.write(data, writeSize);
			if(mFileStream.fail()) {
				FBLOG_DEBUG("DownloadFileTransferAPI::threadFct", "File write error");
				onError("File write error");
				throw std::runtime_error("File write error");
			}
			data += writeSize;
			size -= writeSize;
		}
		mFileStream.close();
		onSuccess(true);
	} catch(std::runtime_error&) {
		// Remove incomplete file
		mFileStream.close();
		boost::filesystem::remove(mFilePath);
	}
	
	detachThread(boost::this_thread::get_id());
	mThread.reset();
}

void DownloadFileTransferAPI::cancel() {
	FBLOG_DEBUG("DownloadFileTransferAPI::cancel", "this=" << this);
	if(mHelper) {
		mHelper->cancel();
	}
	if(mThread) {
		mThread->interrupt();
	}
}

long long DownloadFileTransferAPI::getTransferedBytes() {
	FBLOG_DEBUG("DownloadFileTransferAPI::getTransferedBytes", "this=" << this);
	if(mHelper) {
		mHelper->getReceived();
	}
	return -1;
}

long long DownloadFileTransferAPI::getTotalBytes() {
	FBLOG_DEBUG("DownloadFileTransferAPI::getTotalBytes", "this=" << this);
	if(mHelper) {
		mHelper->getStream()->getLength();
	}
	return -1;
}
	
} // LinphoneWeb
