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

#ifndef H_FILETRANSFERAPI
#define H_FILETRANSFERAPI

#include <linphonecore.h>
#include "../wrapperapi.h"

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
	FileTransferAPI(const FB::URI &sourceUri, const FB::URI &targetUri, const FB::JSObjectPtr& callback);
	virtual void onSuccess(bool done);
	virtual void onError(const std::string &error);
	void initProxy();
	
public:
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
