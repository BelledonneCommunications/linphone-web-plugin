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

#include "localfiletransferapi.h"
#include "../coreplugin.h"
#include "../factoryapi.h"
#include <boost/thread/thread.hpp>

const unsigned int LocalFileTransferAPI::BUFFER_SIZE = 32 * 1024;

LocalFileTransferAPI::LocalFileTransferAPI(const FB::URI &sourceUri, const FB::URI &targetUri, const FB::JSObjectPtr& callback):
    FileTransferAPI(sourceUri, targetUri, callback), mTotalBytes(-1), mTransferedBytes(-1), mStop(false) {
    
}

void LocalFileTransferAPI::start() {
    FBLOG_DEBUG("LocalFileTransferAPI::start()", "this=" << this);
    
    std::string sourceFileStr = mFactory->getFileManager()->uriToFile(mSourceUri);
    if(sourceFileStr.empty()) {
        FBLOG_DEBUG("LocalFileTransferAPI::start()", "Invalid source path");
        onError("Invalid source path");
        return;
    }
    FBLOG_DEBUG("LocalFileTransferAPI::start()", "sourceFile=" << sourceFileStr);
    
    std::string targetFileStr = mFactory->getFileManager()->uriToFile(mTargetUri);
    if(targetFileStr.empty()) {
        FBLOG_DEBUG("LocalFileTransferAPI::start()", "Invalid target path");
        onError("Invalid target path");
        return;
    }
    FBLOG_DEBUG("LocalFileTransferAPI::start()", "targetFile=" << targetFileStr);
    
    boost::filesystem::path sourceFile(sourceFileStr);
    if(!boost::filesystem::exists(sourceFile)) {
        FBLOG_DEBUG("LocalFileTransferAPI::start()", "The source path \"" << sourceFileStr << "\" doesn't exist");
        onError("The source path doesn't exist");
        return;
    }
    
    mTotalBytes = boost::filesystem::file_size(sourceFile);
    
    boost::filesystem::path targetFile(targetFileStr);
    if(boost::filesystem::exists(targetFile)) {
        FBLOG_DEBUG("LocalFileTransferAPI::start()", "The target path \"" << targetFileStr << "\" already exists");
        onError("The target path already exists");
        return;
    }
    
    mSourceFileStream.open(sourceFileStr.c_str(), std::ios_base::in | std::ios_base::binary);
    if(mSourceFileStream.fail()) {
        FBLOG_DEBUG("LocalFileTransferAPI::start()", "Can't open the source file: " << sourceFileStr);
        onError("Can't open the source file");
        return;
    }
    
    mTargetFileStream.open(targetFileStr.c_str(), std::ios_base::out | std::ios_base::binary);
    if(mTargetFileStream.fail()) {
        FBLOG_DEBUG("LocalFileTransferAPI::start()", "Can't open the target file: " << targetFileStr);
        onError("Can't open the target file");
        return;
    }
    
    mTransferedBytes = 0;
    self = boost::static_pointer_cast<LocalFileTransferAPI>(shared_from_this());
    
    // Run thread
    boost::thread t(boost::bind(&LocalFileTransferAPI::threadFct, this));
}

void LocalFileTransferAPI::onSuccess(bool done) {
    FileTransferAPI::onSuccess(done);
    self.reset();
}

void LocalFileTransferAPI::onError(const std::string &error) {
    FileTransferAPI::onError(error);
    self.reset();
}

void LocalFileTransferAPI::threadFct() {
    FBLOG_DEBUG("LocalFileTransferAPI::threadFct()", "this=" << this);
    char buffer[BUFFER_SIZE];
    size_t readSize = 1;
    try {
        mMutex.lock();
        while(!mStop && readSize > 0) {
            mMutex.unlock();
            
            readSize = mSourceFileStream.readsome(buffer, BUFFER_SIZE);
            if(mSourceFileStream.fail()) {
                mSourceFileStream.close();
                mTargetFileStream.close();
                FBLOG_DEBUG("LocalFileTransferAPI::threadFct()", "this=" << this << " | Can't read the source file" << mSourceFileStream.rdstate());
                onError("Can't read the source file");
                return;
            }
            FBLOG_DEBUG("LocalFileTransferAPI::threadFct()", "this=" << this << " | Read " << readSize << "(" << mTransferedBytes << " of " << mTotalBytes << ")");
            mTransferedBytes += readSize;
            mTargetFileStream.write(buffer, readSize);
            if(mTargetFileStream.fail()) {
                mSourceFileStream.close();
                mTargetFileStream.close();
                FBLOG_DEBUG("LocalFileTransferAPI::threadFct()", "this=" << this << " | Can't write the target file " << mTargetFileStream.rdstate());
                onError("Can't write the target file");
                return;
            }
            mMutex.lock();
        }
        mMutex.unlock();
        FBLOG_DEBUG("LocalFileTransferAPI::threadFct()", "this=" << this << " | Done");
        onSuccess(true);
    } catch(std::runtime_error&) {
        // Nothing to do
    }
}

void LocalFileTransferAPI::cancel() {
    FBLOG_DEBUG("LocalFileTransferAPI::cancel()", "this=" << this);
    mMutex.lock();
    mStop = true;
    mMutex.unlock();
}

int LocalFileTransferAPI::getTransferedBytes() {
    FBLOG_DEBUG("LocalFileTransferAPI::getTransferedBytes()", "this=" << this);
    return -1;
}

int LocalFileTransferAPI::getTotalBytes() {
    FBLOG_DEBUG("LocalFileTransferAPI::getTotalBytes()", "this=" << this);
    return -1;
}