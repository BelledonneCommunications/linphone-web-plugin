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

#include "lpconfigapi.h"

#include "utils.h"
#include "factoryapi.h"

namespace LinphoneWeb {

LpConfigAPI::LpConfigAPI(const LpConfig *config) :
		WrapperAPI(APIDescription(this)), mLpConfig(const_cast<LpConfig *>(config)) {
	FBLOG_DEBUG("LpConfigAPI::LpConfigAPI", "this=" << this << "\t" << "config=" << config);
}

LpConfigAPI::LpConfigAPI(StringPtr const &configFilename) :
		WrapperAPI(APIDescription(this)) {
	FBLOG_DEBUG("LpConfigAPI::LpConfigAPI", "this=" << this << "\t" << "configFilename=" << configFilename);
	mLpConfig = lp_config_new(STRING_TO_CHARPTR(configFilename));
	if (mLpConfig == NULL) {
		throw std::invalid_argument("one/many parameters is/are invalid");
	}
}

LpConfigAPI::LpConfigAPI(StringPtr const &configFilename, StringPtr const &factoryConfigFilename) :
		WrapperAPI(APIDescription(this)) {
	FBLOG_DEBUG("LpConfigAPI::LpConfigAPI", "this=" << this << "\t" << "configFilename=" << configFilename << "\t" << "factoryConfigFilename=" << factoryConfigFilename);
	mLpConfig = lp_config_new_with_factory(STRING_TO_CHARPTR(configFilename), STRING_TO_CHARPTR(factoryConfigFilename));
	if (mLpConfig == NULL) {
		throw std::invalid_argument("one/many parameters is/are invalid");
	}
}

void LpConfigAPI::initProxy() {
	registerMethod("readFile", make_method(this, &LpConfigAPI::readFile));
	registerMethod("sync", make_method(this, &LpConfigAPI::sync));
	registerMethod("hasSection", make_method(this, &LpConfigAPI::hasSection));
	registerMethod("cleanSection", make_method(this, &LpConfigAPI::cleanSection));

	registerMethod("getFloat", make_method(this, &LpConfigAPI::getFloat));
	registerMethod("getInt", make_method(this, &LpConfigAPI::getInt));
	registerMethod("getRange", make_method(this, &LpConfigAPI::getRange));
	registerMethod("getString", make_method(this, &LpConfigAPI::getString));
	registerMethod("setFloat", make_method(this, &LpConfigAPI::setFloat));
	registerMethod("setInt", make_method(this, &LpConfigAPI::setInt));
	registerMethod("setIntHex", make_method(this, &LpConfigAPI::setIntHex));
	registerMethod("setRange", make_method(this, &LpConfigAPI::setRange));
	registerMethod("setString", make_method(this, &LpConfigAPI::setString));
}

LpConfigAPI::~LpConfigAPI() {
	FBLOG_DEBUG("LpConfigAPI::~LpConfigAPI", "this=" << this);
	if (isOwned()) {
		if(mLpConfig != NULL) {
			lp_config_destroy(mLpConfig);
		}
	}
}

int LpConfigAPI::readFile(StringPtr const &configFilename) {
	CORE_MUTEX

	FBLOG_DEBUG("LpConfigAPI::readFile", "this=" << this << "\t" << "configFilename=" << configFilename);
	return lp_config_read_file(mLpConfig, STRING_TO_CHARPTR(configFilename));
}

int LpConfigAPI::sync() {
	CORE_MUTEX

	FBLOG_DEBUG("LpConfigAPI::sync", "this=" << this);
	return lp_config_sync(mLpConfig);
}

bool LpConfigAPI::hasSection(StringPtr const &section) const {
	CORE_MUTEX

	FBLOG_DEBUG("LpConfigAPI::hasSection", "this=" << this << "\t" << "section=" << section);
	return (lp_config_has_section(mLpConfig, STRING_TO_CHARPTR(section)) == TRUE) ? true : false;
}

void LpConfigAPI::cleanSection(StringPtr const &section) {
	CORE_MUTEX

	FBLOG_DEBUG("LpConfigAPI::cleanSection", "this=" << this << "\t" << "section=" << section);
	lp_config_clean_section(mLpConfig, STRING_TO_CHARPTR(section));
}


float LpConfigAPI::getFloat(StringPtr const &section, StringPtr const &key, float defaultValue) const {
	CORE_MUTEX

	FBLOG_DEBUG("LpConfigAPI::getFloat", "this=" << this << "\t" << "section=" << section << "\t" << "key=" << key << "\t" << "defaultValue=" << defaultValue);
	return lp_config_get_float(mLpConfig, STRING_TO_CHARPTR(section), STRING_TO_CHARPTR(key), defaultValue);
}

int LpConfigAPI::getInt(StringPtr const &section, StringPtr const &key, int defaultValue) const {
	CORE_MUTEX

	FBLOG_DEBUG("LpConfigAPI::getInt", "this=" << this << "\t" << "section=" << section << "\t" << "key=" << key << "\t" << "defaultValue=" << defaultValue);
	return lp_config_get_int(mLpConfig, STRING_TO_CHARPTR(section), STRING_TO_CHARPTR(key), defaultValue);
}

FB::VariantList LpConfigAPI::getRange(StringPtr const &section, StringPtr const &key, FB::VariantList const &defaultValues) const {
	CORE_MUTEX

	FBLOG_DEBUG("LpConfigAPI::getRange", "this=" << this);
	if (defaultValues.size() != 2) {
		throw std::invalid_argument("one/many parameters is/are invalid");
	}

	int defaultMin = defaultValues[0].cast<int>();
	int defaultMax = defaultValues[1].cast<int>();
	int min, max;
	bool res = (lp_config_get_range(mLpConfig, STRING_TO_CHARPTR(section), STRING_TO_CHARPTR(key), &min, &max, defaultMin, defaultMax) == TRUE) ? true : false;
	if (!res) {
		return FB::VariantList();
	}
	return FB::variant_list_of(min)(max);
}

StringPtr LpConfigAPI::getString(StringPtr const &section, StringPtr const &key, StringPtr const &defaultValue) const {
	CORE_MUTEX

	FBLOG_DEBUG("LpConfigAPI::getString", "this=" << this << "\t" << "section=" << section << "\t" << "key=" << key << "\t" << "defaultValue=" << defaultValue);
	return CHARPTR_TO_STRING(lp_config_get_string(mLpConfig, STRING_TO_CHARPTR(section), STRING_TO_CHARPTR(key), STRING_TO_CHARPTR(defaultValue)));
}

void LpConfigAPI::setFloat(StringPtr const &section, StringPtr const &key, float value) {
	CORE_MUTEX

	FBLOG_DEBUG("LpConfigAPI::setFloat", "this=" << this << "\t" << "section=" << section << "\t" << "key=" << key << "\t" << "value=" << value);
	lp_config_set_float(mLpConfig, STRING_TO_CHARPTR(section), STRING_TO_CHARPTR(key), value);
}

void LpConfigAPI::setInt(StringPtr const &section, StringPtr const &key, int value) {
	CORE_MUTEX

	FBLOG_DEBUG("LpConfigAPI::setInt", "this=" << this << "\t" << "section=" << section << "\t" << "key=" << key << "\t" << "value=" << value);
	lp_config_set_int(mLpConfig, STRING_TO_CHARPTR(section), STRING_TO_CHARPTR(key), value);
}

void LpConfigAPI::setIntHex(StringPtr const &section, StringPtr const &key, int value) {
	CORE_MUTEX

	FBLOG_DEBUG("LpConfigAPI::setIntHex", "this=" << this << "\t" << "section=" << section << "\t" << "key=" << key << "\t" << "value=" << value);
	lp_config_set_int_hex(mLpConfig, STRING_TO_CHARPTR(section), STRING_TO_CHARPTR(key), value);
}

void LpConfigAPI::setRange(StringPtr const &section, StringPtr const &key, FB::VariantList const &values) {
	CORE_MUTEX

	FBLOG_DEBUG("LpConfigAPI::setRange", "this=" << this << "\t" << "section=" << section << "\t" << "key=" << key);
	if (values.size() != 2) {
		throw std::invalid_argument("one/many parameters is/are invalid");
	}

	int min = values[0].cast<int>();
	int max = values[1].cast<int>();
	lp_config_set_range(mLpConfig, STRING_TO_CHARPTR(section), STRING_TO_CHARPTR(key), min, max);
}

void LpConfigAPI::setString(StringPtr const &section, StringPtr const &key, StringPtr const &value) {
	CORE_MUTEX

	FBLOG_DEBUG("LpConfigAPI::setString", "this=" << this << "\t" << "section=" << section << "\t" << "key=" << key << "\t" << "value=" << value);
	lp_config_set_string(mLpConfig, STRING_TO_CHARPTR(section), STRING_TO_CHARPTR(key), STRING_TO_CHARPTR(value));
}

} // LinphoneWeb
