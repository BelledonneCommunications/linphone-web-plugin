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

#include "localfiletransferapi.h"
#include "../Src/coreplugin.h"
#include "../Src/factoryapi.h"
#include <boost/thread/thread.hpp>

namespace LinphoneWeb {

const unsigned int LocalFileTransferAPI::BUFFER_SIZE = 32 * 1024;

LocalFileTransferAPI::LocalFileTransferAPI(FB::URI const &sourceUri, FB::URI const &targetUri, FB::JSObjectPtr const &callback):
	FileTransferAPI(sourceUri, targetUri, callback), mTotalBytes(-1), mTransferedBytes(-1) {
	FBLOG_DEBUG("LocalFileTransferAPI::LocalFileTransferAPI", "this=" << this);
}

LocalFileTransferAPI::~LocalFileTransferAPI() {
	FBLOG_DEBUG("LocalFileTransferAPI::~LocalFileTransferAPI", "this=" << this);
}

void LocalFileTransferAPI::start() {
	FBLOG_DEBUG("LocalFileTransferAPI::start", "this=" << this);
	
	mSourceFileStr = getFactory()->getFileManager()->uriToFile(mSourceUri);
	if(mSourceFileStr.empty()) {
		FBLOG_DEBUG("LocalFileTransferAPI::start", "Invalid source path");
		onError("Invalid source path");
		return;
	}
	FBLOG_DEBUG("LocalFileTransferAPI::start", "sourceFile=" << mSourceFileStr);
	
	mTargetFileStr = getFactory()->getFileManager()->uriToFile(mTargetUri);
	if(mTargetFileStr.empty()) {
		FBLOG_DEBUG("LocalFileTransferAPI::start", "Invalid target path");
		onError("Invalid target path");
		return;
	}
	FBLOG_DEBUG("LocalFileTransferAPI::start", "targetFile=" << mTargetFileStr);
	
	mSourceFilePath = boost::filesystem::path(mSourceFileStr);
	if(!boost::filesystem::exists(mSourceFilePath)) {
		FBLOG_DEBUG("LocalFileTransferAPI::start", "The source path \"" << mSourceFileStr << "\" doesn't exist");
		onError("The source path doesn't exist");
		return;
	}
	
	mTargetFilePath = boost::filesystem::path(mTargetFileStr);
	if(boost::filesystem::exists(mTargetFilePath)) {
		FBLOG_DEBUG("LocalFileTransferAPI::start", "The target path \"" << mTargetFileStr << "\" already exists");
		onError("The target path already exists");
		return;
	}
	
	mTransferedBytes = 0;
	
	// Run thread
	mThread = boost::make_shared<boost::thread>(boost::bind(&LocalFileTransferAPI::threadFctHolder, boost::static_pointer_cast<LocalFileTransferAPI>(shared_from_this())));
	attachThread(mThread);
}

void LocalFileTransferAPI::onSuccess(bool done) {
	FileTransferAPI::onSuccess(done);
}

void LocalFileTransferAPI::onError(std::string const &error) {
	FileTransferAPI::onError(error);
}

void LocalFileTransferAPI::threadFctHolder(LocalFileTransferAPIPtr &self) {
	self->threadFct();
}

void LocalFileTransferAPI::threadFct() {
	FBLOG_DEBUG("LocalFileTransferAPI::threadFct", "this=" << this);
	char buffer[BUFFER_SIZE];
	std::streamsize readSize = 1;
	try {
		mSourceFileStream.open(mSourceFileStr.c_str(), std::ios_base::in | std::ios_base::binary);
		if(mSourceFileStream.fail()) {
			FBLOG_DEBUG("LocalFileTransferAPI::start", "Can't open the source file: " << mSourceFileStr);
			onError("Can't open the source file");
			throw std::runtime_error("Can't open the source file");
		}
	
		mTargetFileStream.open(mTargetFileStr.c_str(), std::ios_base::out | std::ios_base::binary);
		if(mTargetFileStream.fail()) {
			FBLOG_DEBUG("LocalFileTransferAPI::start", "Can't open the target file: " << mTargetFileStr);
			onError("Can't open the target file");
			throw std::runtime_error("Can't open the target file");
		}
		mSourceFileStream.seekg (0, std::ifstream::end);
		mTotalBytes = mSourceFileStream.tellg();
		mSourceFileStream.seekg (0, std::ifstream::beg);

		boost::this_thread::disable_interruption di;
		while(!boost::this_thread::interruption_requested() && readSize > 0) {
			readSize = mTotalBytes - mTransferedBytes;
			if(readSize > BUFFER_SIZE) readSize = BUFFER_SIZE;
			mSourceFileStream.read(buffer, readSize);
			readSize = mSourceFileStream.gcount();
			if(mSourceFileStream.fail()) {
				FBLOG_DEBUG("LocalFileTransferAPI::threadFct", "this=" << this << " | Can't read the source file" << mSourceFileStream.rdstate());
				onError("Can't read the source file");
				throw std::runtime_error("Can't read the source file");
			}
			FBLOG_DEBUG("LocalFileTransferAPI::threadFct", "this=" << this << " | Read " << readSize << "(" << mTransferedBytes << " of " << mTotalBytes << ")");
			mTransferedBytes += readSize;
			mTargetFileStream.write(buffer, readSize);
			if(mTargetFileStream.fail()) {
				FBLOG_DEBUG("LocalFileTransferAPI::threadFct", "this=" << this << " | Can't write the target file " << mTargetFileStream.rdstate());
				onError("Can't write the target file");
				throw std::runtime_error("Can't write the target file");
			}
		}
		if(mTransferedBytes != mTotalBytes) {
			FBLOG_DEBUG("LocalFileTransferAPI::threadFct", "this=" << this << " | Incomplet transfer");
			onError("Incomplet transfer");
			throw std::runtime_error("Incomplet transfer");
		}

		mSourceFileStream.close();
		mTargetFileStream.close();
		FBLOG_DEBUG("LocalFileTransferAPI::threadFct", "this=" << this << " | Done");
		onSuccess(true);
	} catch(std::runtime_error&) {
		mSourceFileStream.close();
		mTargetFileStream.close();
		// Nothing to do
	}
	detachThread(boost::this_thread::get_id());
	mThread.reset();
}

void LocalFileTransferAPI::cancel() {
	FBLOG_DEBUG("LocalFileTransferAPI::cancel", "this=" << this);
	if(mThread) {
		mThread->interrupt();
	}
}

long long LocalFileTransferAPI::getTransferedBytes() {
	FBLOG_DEBUG("LocalFileTransferAPI::getTransferedBytes", "this=" << this);
	return mTransferedBytes;
}

long long LocalFileTransferAPI::getTotalBytes() {
	FBLOG_DEBUG("LocalFileTransferAPI::getTotalBytes", "this=" << this);
	return mTotalBytes;
}
	
} // LinphoneWeb
