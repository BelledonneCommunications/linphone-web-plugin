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

#include "uploadfiletransferapi.h"
#include "../coreplugin.h"
#include "../factoryapi.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include "../base64.h"

const unsigned int UploadFileTransferAPI::BUFFER_SIZE = 4096;

UploadFileTransferAPI::UploadFileTransferAPI(const FB::URI &sourceUri, const FB::URI &targetUri, const FB::JSObjectPtr& callback):
    FileTransferAPI(sourceUri, targetUri, callback) {
}

void UploadFileTransferAPI::start() {
    FBLOG_DEBUG("UploadFileTransferAPI::start()", "this=" << this);
    std::string fileStr = mFactory->getFileManager()->uriToFile(mSourceUri);
    if(fileStr.empty()) {
        FBLOG_DEBUG("UploadFileTransferAPI::start()", "Invalid source path");
        onError("Invalid source path");
        return;
    }
    FBLOG_DEBUG("UploadFileTransferAPI::start()", "sourceFile=" << fileStr);
    
    boost::filesystem::path file(fileStr);
    if(!boost::filesystem::exists(file)) {
        FBLOG_DEBUG("UploadFileTransferAPI::start()", "The path source \"" << fileStr << "\" doesn't exist");
        onError("The source path doesn't exist");
        return;
    }
    if(boost::filesystem::is_directory(file)) {
        FBLOG_DEBUG("UploadFileTransferAPI::start()", "Can't upload a directory");
        onError("Can't transfer a directory");
        return;
    }
    
    std::ifstream fileStream;
    fileStream.open(fileStr.c_str(), std::ios_base::in | std::ios_base::binary);
    if(fileStream.fail()) {
        FBLOG_DEBUG("UploadFileTransferAPI::start()", "Can't open the source file: " << fileStr);
        onError("Can't open the source file");
        return;
    }
    std::string binaryData((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
    if(fileStream.fail()) {
        FBLOG_DEBUG("UploadFileTransferAPI::start()", "Can't read the source file: " << fileStr);
        onError("Can't read the source file");
        return;
    }
    fileStream.close();
    
    std::string base64Data = base64_encode(binaryData);
    
    // Create the POST data
    std::string postData;
    postData += "filename=";
    postData += file.filename().string();
    postData += "&data=";
    postData += base64Data;
    
    FBLOG_DEBUG("UploadFileTransferAPI::start()", fileStr << "(" << binaryData.length() << "->" << base64Data.length() <<")");
    
    // Create the request
    FB::BrowserStreamRequest req(mTargetUri, "POST");
    req.setPostData(postData);
    req.setCallback(boost::bind(&UploadFileTransferAPI::callbackFct, this, _1, _2, _3, _4));

    // Start transfer
    FB::BrowserHostPtr host = mFactory->getPlugin()->getHost();
    mHelper = FileStreamHelper::AsyncRequest(host, req);
}

void UploadFileTransferAPI::callbackFct(bool success, const FB::HeaderMap& headers, const boost::shared_array<uint8_t>& data, const size_t size) {
    FBLOG_DEBUG("UploadFileTransferAPI::callbackFct()", "this=" << this);
    if(!success) {
        FBLOG_DEBUG("UploadFileTransferAPI::callbackFct()", "HTTP error");
        onError("HTTP error");
        return;
    }
    onSuccess(success);
}

void UploadFileTransferAPI::cancel() {
    FBLOG_DEBUG("UploadFileTransferAPI::cancel()", "this=" << this);
    if(mHelper) {
        mHelper->cancel();
    }
}

int UploadFileTransferAPI::getTransferedBytes() {
    FBLOG_DEBUG("UploadFileTransferAPI::getTransferedBytes()", "this=" << this);
    if(mHelper) {
        mHelper->getReceived();
    }
    return -1;
}

int UploadFileTransferAPI::getTotalBytes() {
    FBLOG_DEBUG("UploadFileTransferAPI::getTotalBytes()", "this=" << this);
    if(mHelper) {
        mHelper->getStream()->getLength();
    }
    return -1;
}

