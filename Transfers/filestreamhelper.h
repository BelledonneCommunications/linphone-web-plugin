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

#ifndef H_FILESTREAMHELPER
#define H_FILESTREAMHELPER

#include <BrowserStreamRequest.h>
#include <SimpleStreamHelper.h>

FB_FORWARD_PTR(FileStreamHelper);
class FileStreamHelper: public FB::SimpleStreamHelper {
public:
	static FileStreamHelperPtr AsyncRequest(const FB::BrowserHostConstPtr& host, const FB::BrowserStreamRequest& req) {
	    if (!req.getCallback()) {
	        throw std::runtime_error("Invalid callback");
	    }
	    if (!host->isMainThread()) {
	        // This must be run from the main thread
	        return host->CallOnMainThread(boost::bind(&AsyncRequest, host, req));
	    }
	    FB::BrowserStreamPtr stream(host->createStream(req, false));
	    if (!stream) {
	        throw std::runtime_error("Invalid stream");
	    }
	    return AsyncRequest(host, stream, req);
	}
	
	static FileStreamHelperPtr AsyncRequest(const FB::BrowserHostConstPtr& host, const FB::BrowserStreamPtr& stream, const FB::BrowserStreamRequest& req) {
	    if (!host->isMainThread()) {
	        // This must be run from the main thread
	        return host->CallOnMainThread(boost::bind(&AsyncRequest, host, stream, req));
	    }
	    FileStreamHelperPtr ptr(boost::make_shared<FileStreamHelper>(req.getCallback(), req.internalBufferSize));
	    // This is kinda a weird trick; it's responsible for freeing itself, unless something decides
	    // to hold a reference to it.
	    ptr->keepReference(ptr);
	    stream->AttachObserver(ptr);
	    return ptr;
	}
	FileStreamHelper(const FB::HttpCallback& callback, const size_t blockSize):
	    FB::SimpleStreamHelper(callback, blockSize) {
	    
	}
	
	size_t getReceived() {
	    return received;
	}
	
protected:
	bool onStreamCompleted(FB::StreamCompletedEvent *evt, FB::BrowserStream *stream) {
	    bool ret = FB::SimpleStreamHelper::onStreamCompleted(evt, stream);
	    self.reset();
	    return ret;
	}
	
private:
	void keepReference(const FileStreamHelperPtr& ptr) {
	    self = ptr;
	}
	FileStreamHelperPtr self;
};

#endif //H_FILESTREAMHELPER
