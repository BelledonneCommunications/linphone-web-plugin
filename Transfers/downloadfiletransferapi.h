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

#ifndef H_DOWNLOADFILETRANSFERAPI
#define H_DOWNLOADFILETRANSFERAPI

#include "filetransferapi.h"
#include "filestreamhelper.h"
#include <fstream>
#include <boost/filesystem.hpp>

FB_FORWARD_PTR(DownloadFileTransferAPI)
class DownloadFileTransferAPI: public FileTransferAPI {
	friend class FactoryAPI;
private:
	FileStreamHelperPtr mHelper;
	static const unsigned int BUFFER_SIZE;
	
	std::string mFileStr;
	boost::filesystem::path mFilePath;
	std::ofstream mFileStream;
	
	boost::shared_ptr<boost::thread> mThread;
	
private:
	DownloadFileTransferAPI(const FB::URI &sourceUri, const FB::URI &targetUri, const FB::JSObjectPtr& callback);
	void callbackFct(bool success, const FB::HeaderMap& headers, const boost::shared_array<uint8_t>& data, const size_t size);
	void threadFct(const boost::shared_array<uint8_t> &data, size_t &size);
	static void threadFctHolder(DownloadFileTransferAPIPtr &self, const boost::shared_array<uint8_t> &data, size_t &size);
	
public:
	~DownloadFileTransferAPI();
	virtual void start();
	virtual void cancel();
	virtual int getTransferedBytes();
	virtual int getTotalBytes();
};

#endif //H_DOWNLOADFILETRANSFERAPI
