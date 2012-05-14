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

#ifndef H_VIDEOAPI
#define H_VIDEOAPI

#include <string>
#include <JSAPIAuto.h>
#include <BrowserHost.h>
#include <variant_list.h>
#include "videoplugin.h"
#include "videowindow.h"

class VideoAPI: public FB::JSAPIAuto {
public:
	VideoAPI(const videoPtr& plugin, const FB::BrowserHostPtr& host);
	~VideoAPI();

	void setWindow(FB::PluginWindow *window);
	videoPtr getPlugin();

	// Property
	const std::string &getMagic();
	void setMagic(const std::string &magic);
	unsigned long getWindow();

	void setBackgroundColor(int r, int g, int b);

private:
	std::string m_magic;
	videoWeakPtr m_plugin;
	FB::BrowserHostPtr m_host;
	boost::shared_ptr<VideoWindow> mWindow;
};

#endif // H_VIDEOAPI
