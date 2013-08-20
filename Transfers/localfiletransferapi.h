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

#ifndef H_LOCALFILETRANSFERAPI
#define H_LOCALFILETRANSFERAPI

#include "filetransferapi.h"
#include <fstream>
#include <boost/filesystem.hpp>

namespace LinphoneWeb {

FB_FORWARD_PTR(LocalFileTransferAPI)
class LocalFileTransferAPI: public FileTransferAPI {
	friend class FactoryAPI;
	friend class FileTransferAPI;
	
private:
	LocalFileTransferAPIPtr self;
	static const unsigned int BUFFER_SIZE;
	std::string mTargetFileStr;
	boost::filesystem::path mTargetFilePath;
	std::ofstream mTargetFileStream;
	std::string mSourceFileStr;
	boost::filesystem::path mSourceFilePath;
	std::ifstream mSourceFileStream;
	long long mTotalBytes;
	long long mTransferedBytes;
	boost::shared_ptr<boost::thread> mThread;
	
	LocalFileTransferAPI(FB::URI const &sourceUri, FB::URI const &targetUri, FB::JSObjectPtr const &callback);
	static void threadFctHolder(LocalFileTransferAPIPtr &self);
	void threadFct();
	
protected:
	virtual void onSuccess(bool done);
	virtual void onError(std::string const &error);
	
public:
	virtual ~LocalFileTransferAPI();
	
	virtual void start();
	virtual void cancel();
	virtual long long getTransferedBytes();
	virtual long long getTotalBytes();
};
	
} // LinphoneWeb

#endif //H_LOCALFILETRANSFERAPI
