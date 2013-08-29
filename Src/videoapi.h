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

#ifndef H_VIDEOAPI
#define H_VIDEOAPI

#include <string>
#include "videowindow.h"

#include "wrapperapi.h"

namespace LinphoneWeb {
	
FB_FORWARD_PTR(PointerAPI)

FB_FORWARD_PTR(VideoAPI)
class VideoAPI: public WrapperAPI {
	friend class FactoryAPI;
public:
	typedef boost::function<void (void *ptr)> WindowEventHandler;
	VideoAPI();
	~VideoAPI();

	void setWindow(FB::PluginWindow *window);

	// Property
	std::string const &getMagic();
	void setMagic(std::string const &magic);
	WhiteBoard::IdType getWindow();

	void setWindowEventHandler(WindowEventHandler const &windowEventHandler);
	
	void setBackgroundColor(int r, int g, int b);
	bool draw();
protected:
	virtual void initProxy();
	
private:
	WindowEventHandler mWindowEventHandler;
	WhiteBoard::IdType mId;
	std::string mMagic;
	VideoWindowPtr mWindow;
};
	
} // LinphoneWeb

#endif // H_VIDEOAPI
