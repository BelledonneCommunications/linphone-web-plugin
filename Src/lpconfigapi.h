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
 - Ghislain MARY <ghislain.mary@belledonne-communications.com>

 */

#ifndef H_LPCONFIGAPI
#define H_LPCONFIGAPI

#include <linphone/lpconfig.h>
#include "wrapperapi.h"

namespace LinphoneWeb {

FB_FORWARD_PTR(LpConfigAPI)

class LpConfigAPI: public WrapperAPI {
	friend class FactoryAPI;

private:
	LpConfig *mLpConfig;

	LpConfigAPI(const LpConfig *lpconfig);
	LpConfigAPI(StringPtr const &configFilename);
	LpConfigAPI(StringPtr const &configFilename, StringPtr const &factoryConfigFilename);

protected:
	virtual void initProxy();

public:
	virtual ~LpConfigAPI();

	int readFile(StringPtr const &configFilename);
	int sync();
	bool hasSection(StringPtr const &section) const;
	void cleanSection(StringPtr const &section);

	float getFloat(StringPtr const &section, StringPtr const &key, float defaultValue) const;
	int getInt(StringPtr const &section, StringPtr const &key, int defaultValue) const;
	FB::VariantList getRange(StringPtr const &section, StringPtr const &key, FB::VariantList const &defaultValues) const;
	StringPtr getString(StringPtr const &section, StringPtr const &key, StringPtr const &defaultValue) const;

	void setFloat(StringPtr const &section, StringPtr const &key, float value);
	void setInt(StringPtr const &section, StringPtr const &key, int value);
	void setIntHex(StringPtr const &section, StringPtr const &key, int value);
	void setRange(StringPtr const &section, StringPtr const &key, FB::VariantList const &values);
	void setString(StringPtr const &section, StringPtr const &key, StringPtr const &value);

	inline LpConfig *getRef() {
		return mLpConfig;
	}
};

} // LinphoneWeb
	
#endif //H_LPCONFIGAPI
