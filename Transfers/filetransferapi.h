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

#ifndef H_FILETRANSFERAPI
#define H_FILETRANSFERAPI

#include "../Src/wrapperapi.h"

#include <URI.h>

namespace LinphoneWeb {

FB_FORWARD_PTR(FileTransferAPI)
class FileTransferAPI: public WrapperAPI {
	friend class FactoryAPI;
protected:
	const FB::URI mSourceUri;
	const FB::URI mTargetUri;
	const FB::JSObjectPtr mCallback;
	bool mDone;
	std::string mError;
	
protected:
	FileTransferAPI(FB::URI const &sourceUri, FB::URI const &targetUri, FB::JSObjectPtr const &callback);
	virtual void onSuccess(bool done);
	virtual void onError(std::string const &error);
	virtual void initProxy();
	
public:
	static FileTransferAPIPtr create(FB::URI const &sourceUri, FB::URI const &targetUri, FB::JSObjectPtr const &callback);
	virtual void start() = 0;
	virtual void cancel() = 0;
	std::string getError();
	bool isDone();
	virtual long long getTransferedBytes() = 0;
	virtual long long getTotalBytes() = 0;
	std::string getSourceUrl();
	std::string getTargetUrl();
};
	
} // LinphoneWeb

#endif //H_FILETRANSFERAPI
