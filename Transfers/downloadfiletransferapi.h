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

#ifndef H_DOWNLOADFILETRANSFERAPI
#define H_DOWNLOADFILETRANSFERAPI

#include "filetransferapi.h"
#include "filestreamhelper.h"
#include <fstream>
#include <boost/filesystem.hpp>

namespace LinphoneWeb {

FB_FORWARD_PTR(DownloadFileTransferAPI)
class DownloadFileTransferAPI: public FileTransferAPI {
	friend class FactoryAPI;
	friend class FileTransferAPI;
	
private:
	FileStreamHelperPtr mHelper;
	static const unsigned int BUFFER_SIZE;
	
	std::string mFileStr;
	boost::filesystem::path mFilePath;
	std::ofstream mFileStream;
	
	boost::shared_ptr<boost::thread> mThread;
	
private:
	DownloadFileTransferAPI(FB::URI const &sourceUri, FB::URI const &targetUri, FB::JSObjectPtr const &callback);
	void callbackFct(bool success, FB::HeaderMap const &headers, boost::shared_array<uint8_t> const &data, const size_t size);
	void threadFct(boost::shared_array<uint8_t> const &data, size_t &size);
	static void threadFctHolder(DownloadFileTransferAPIPtr &self, boost::shared_array<uint8_t> const &data, size_t &size);
	
public:
	virtual ~DownloadFileTransferAPI();
	
	virtual void start();
	virtual void cancel();
	virtual long long getTransferedBytes();
	virtual long long getTotalBytes();
};

} // LinphoneWeb

#endif //H_DOWNLOADFILETRANSFERAPI
