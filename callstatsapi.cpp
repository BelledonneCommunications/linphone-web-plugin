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

#include "callstatsapi.h"
#include "utils.h"

CallStatsAPI::CallStatsAPI(const LinphoneCallStats *callStats) :
		JSAPIAuto(APIDescription(this)), mCallStats(const_cast<LinphoneCallStats *>(callStats)) {
    mUsed = true;
    mConst = true;
	FBLOG_DEBUG("CallStatsAPI::CallStatsAPI", "this=" << this << "\t" << "callStats=" << callStats);
	initProxy();
}

void CallStatsAPI::initProxy() {
	if (mConst) {
		registerProperty("type", make_property(this, &CallStatsAPI::getType));
		registerProperty("roundTripDelay", make_property(this, &CallStatsAPI::getRoundTripDelay));
	} else {
		registerProperty("type", make_property(this, &CallStatsAPI::getType, &CallStatsAPI::setType));
		registerProperty("roundTripDelay", make_property(this, &CallStatsAPI::getRoundTripDelay, &CallStatsAPI::setRoundTripDelay));
	}
}

CallStatsAPI::~CallStatsAPI() {
	FBLOG_DEBUG("CallStatsAPI::~CallStatsAPI", "this=" << this);
}

int CallStatsAPI::getType() const {
	FBLOG_DEBUG("CallStatsAPI::getType", "this=" << this);
	return mCallStats->type;
}

void CallStatsAPI::setType(int type) {
	FBLOG_DEBUG("CallStatsAPI::setType", "this=" << this << "\t" << "type=" << type);
	mCallStats->type = type;
}

float CallStatsAPI::getRoundTripDelay() const {
	FBLOG_DEBUG("CallStatsAPI::getRoundTripDelay", "this=" << this);
	return mCallStats->round_trip_delay;
}

void CallStatsAPI::setRoundTripDelay(float delay) {
	FBLOG_DEBUG("CallStatsAPI::setRoundTripDelay", "this=" << this << "\t" << "delay=" << delay);
	mCallStats->round_trip_delay = delay;
}
