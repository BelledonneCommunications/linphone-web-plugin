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

#include "filetransferapi.h"

#include "downloadfiletransferapi.h"
#include "uploadfiletransferapi.h"
#include "localfiletransferapi.h"
#include "../Src/filemanagerapi.h"

namespace LinphoneWeb {

FileTransferAPIPtr FileTransferAPI::create(FB::URI const &sourceUri, FB::URI const &targetUri, FB::JSObjectPtr const &callback) {
	if(FileManagerAPI::isFile(sourceUri) && FileManagerAPI::isHttp(targetUri)) {
		return FileTransferAPIPtr(new UploadFileTransferAPI(sourceUri, targetUri, callback));
	} else if(FileManagerAPI::isHttp(sourceUri) && FileManagerAPI::isFile(targetUri)) {
		return FileTransferAPIPtr(new DownloadFileTransferAPI(sourceUri, targetUri, callback));
	} else if(FileManagerAPI::isFile(sourceUri) && FileManagerAPI::isFile(targetUri)) {
		return FileTransferAPIPtr(new LocalFileTransferAPI(sourceUri, targetUri, callback));
	}
	
	// Not allowed
	throw std::exception();
}

FileTransferAPI::FileTransferAPI(FB::URI const &sourceUri, FB::URI const &targetUri, FB::JSObjectPtr const &callback):
	WrapperAPI(APIDescription(this)), mSourceUri(sourceUri), mTargetUri(targetUri), mCallback(callback), mDone(false) {
}

void FileTransferAPI::initProxy() {
	registerMethod("start", make_method(this, &FileTransferAPI::start));
	registerMethod("cancel", make_method(this, &FileTransferAPI::cancel));
	registerProperty("error", make_property(this, &FileTransferAPI::getError));
	registerProperty("done", make_property(this, &FileTransferAPI::isDone));
	registerProperty("receivedBytes", make_property(this, &FileTransferAPI::getTransferedBytes));
	registerProperty("totalBytes", make_property(this, &FileTransferAPI::getTotalBytes));
	registerProperty("sourceUrl", make_property(this, &FileTransferAPI::getSourceUrl));
	registerProperty("targetUrl", make_property(this, &FileTransferAPI::getTargetUrl));;
}

void FileTransferAPI::onSuccess(bool done) {
	FBLOG_DEBUG("FileManagerAPI::onSuccess", "this=" << this << "\t" << "done=" << done);
	mDone = done;
	mError.clear();

	mCallback->InvokeAsync("", FB::variant_list_of(done)(FB::variant()));
}

void FileTransferAPI::onError(std::string const &error) {
	FBLOG_DEBUG("FileManagerAPI::onError", "this=" << this << "\t" << "error=" << error);
	mDone = false;
	mError = error;
	
	mCallback->InvokeAsync("", FB::variant_list_of(false)(mError));
}

std::string FileTransferAPI::getError() {
	FBLOG_DEBUG("FileTransferAPI::getError", "this=" << this);
	return mError;
}

bool FileTransferAPI::isDone() {
	FBLOG_DEBUG("FileTransferAPI::isDone", "this=" << this);
	return mDone;
}

std::string FileTransferAPI::getSourceUrl() {
	FBLOG_DEBUG("FileTransferAPI::getSourceUrl", "this=" << this);
	return mSourceUri.toString();
}

std::string FileTransferAPI::getTargetUrl() {
	FBLOG_DEBUG("FileTransferAPI::getTargetUrl", "this=" << this);
	return mTargetUri.toString();
}
	
} // LinphoneWeb
