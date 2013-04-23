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

#ifndef H_LOCALFILETRANSFERAPI
#define H_LOCALFILETRANSFERAPI

#include "filetransferapi.h"
#include <fstream>
#include <boost/thread/mutex.hpp>

FB_FORWARD_PTR(LocalFileTransferAPI)
class LocalFileTransferAPI: public FileTransferAPI {
    friend class FactoryAPI;
    
private:
    LocalFileTransferAPIPtr self;
    static const unsigned int BUFFER_SIZE;
    std::ofstream mTargetFileStream;
    std::ifstream mSourceFileStream;
    int mTotalBytes;
    int mTransferedBytes;
    bool mStop;
    boost::mutex mMutex;
    
    LocalFileTransferAPI(const FB::URI &sourceUri, const FB::URI &targetUri, const FB::JSObjectPtr& callback);
    void threadFct();
protected:
    virtual void onSuccess(bool done);
    virtual void onError(const std::string &error);
    
public:
    virtual void start();
    virtual void cancel();
    virtual int getTransferedBytes();
    virtual int getTotalBytes();
};

#endif //H_LOCALFILETRANSFERAPI