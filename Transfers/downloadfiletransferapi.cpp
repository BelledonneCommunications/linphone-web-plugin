/*!
 Linphone Web - Web plugin of Linphone an audio/video SIP phone
 Copyright (C) 2012  Yann Diorcet <yann.diorcet@linphone.org>
 
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
 */

#include "downloadfiletransferapi.h"
#include "../coreplugin.h"
#include "../factoryapi.h"

DownloadFileTransferAPI::DownloadFileTransferAPI(const FB::URI &sourceUri, const FB::URI &targetUri, const FB::JSObjectPtr& callback):
    FileTransferAPI(sourceUri, targetUri, callback) {
    
}

void DownloadFileTransferAPI::start() {
    FBLOG_DEBUG("DownloadFileTransferAPI::start()", "this=" << this);
    std::string fileStr = mFactory->getFileManager()->uriToFile(mTargetUri);
    if(fileStr.empty()) {
        FBLOG_DEBUG("DownloadFileTransferAPI::start()", "Invalid target path");
        onError("Invalid target path");
        return;
    }
    FBLOG_DEBUG("DownloadFileTransferAPI::start()", "targetFile=" << fileStr);
    
    boost::filesystem::path file(fileStr);
    if(boost::filesystem::exists(file)) {
        FBLOG_DEBUG("DownloadFileTransferAPI::start()", "The path target \"" << fileStr << "\" already exists");
        onError("The target path already exist");
        return;
    }
    
    mFileStream.open(fileStr.c_str(), std::ios_base::out | std::ios_base::binary);
    if(mFileStream.fail()) {
        FBLOG_DEBUG("UploadFileTransferAPI::start()", "Can't open the target file: " << fileStr);
        onError("Can't open the target file");
        return;
    }
    
    // Create the request
    FB::BrowserStreamRequest req(mTargetUri, "GET");
    req.setCallback(boost::bind(&DownloadFileTransferAPI::callbackFct, this, _1, _2, _3, _4));
    
    // Start transfer
    FB::BrowserHostPtr host = mFactory->getPlugin()->getHost();
    mHelper = FileStreamHelper::AsyncRequest(host, req);
}

void DownloadFileTransferAPI::callbackFct(bool success, const FB::HeaderMap& headers, const boost::shared_array<uint8_t>& data, const size_t size) {
    FBLOG_DEBUG("UploadFileTransferAPI::callbackFct()", "this=" << this);
    if(!success) {
        mFileStream.close();
        FBLOG_DEBUG("UploadFileTransferAPI::callbackFct()", "HTTP error");
        onError("HTTP error");
        return;
    }
    mFileStream.write((const char *)data.get(), size);
    if(mFileStream.fail()) {
        mFileStream.close();
        FBLOG_DEBUG("UploadFileTransferAPI::start()", "File write error");
        onError("File write error");
        return;
    }
    mFileStream.close();
    onSuccess(success);
}

void DownloadFileTransferAPI::cancel() {
    FBLOG_DEBUG("DownloadFileTransferAPI::cancel()", "this=" << this);
    if(mHelper) {
        mHelper->cancel();
    }
}

int DownloadFileTransferAPI::getTransferedBytes() {
    FBLOG_DEBUG("DownloadFileTransferAPI::getTransferedBytes()", "this=" << this);
    if(mHelper) {
        mHelper->getReceived();
    }
    return -1;
}

int DownloadFileTransferAPI::getTotalBytes() {
    FBLOG_DEBUG("DownloadFileTransferAPI::getTotalBytes()", "this=" << this);
    if(mHelper) {
        mHelper->getStream()->getLength();
    }
    return -1;
}