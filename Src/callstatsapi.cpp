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

#include "callstatsapi.h"

#include "utils.h"
#include "factoryapi.h"

namespace LinphoneWeb {

CallStatsAPI::CallStatsAPI(const LinphoneCallStats *callStats) :
		WrapperAPI(APIDescription(this)), mCallStats(const_cast<LinphoneCallStats *>(callStats)) {
	FBLOG_DEBUG("CallStatsAPI::CallStatsAPI", "this=" << this << "\t" << "callStats=" << callStats);
}

void CallStatsAPI::initProxy() {
	registerProperty("downloadBandwidth", make_property(this, &CallStatsAPI::getDownloadBandwidth));
	registerProperty("iceStats", make_property(this, &CallStatsAPI::getIceStats));
	registerProperty("localLateRate", make_property(this, &CallStatsAPI::getLocalLateRate));
	registerProperty("localLossRate", make_property(this, &CallStatsAPI::getLocalLossRate));
	registerProperty("roundTripDelay", make_property(this, &CallStatsAPI::getRoundTripDelay));
	registerProperty("type", make_property(this, &CallStatsAPI::getType));
	registerProperty("uploadBandwidth", make_property(this, &CallStatsAPI::getUploadBandwidth));
	registerProperty("upnpState", make_property(this, &CallStatsAPI::getUpnpState));
}

CallStatsAPI::~CallStatsAPI() {
	FBLOG_DEBUG("CallStatsAPI::~CallStatsAPI", "this=" << this);
}

float CallStatsAPI::getDownloadBandwidth() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallStatsAPI::getDownloadBandwidth", "this=" << this);
	return mCallStats->download_bandwidth;
}

int CallStatsAPI::getIceStats() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallStatsAPI::getIceStats", "this=" << this);
	return mCallStats->ice_state;
}

float CallStatsAPI::getLocalLateRate() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallStatsAPI::getLocalLossRate", "this=" << this);
	return mCallStats->local_late_rate;
}

float CallStatsAPI::getLocalLossRate() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallStatsAPI::getLocalLossRate", "this=" << this);
	return mCallStats->local_loss_rate;
}

//receivedRtcp() const;
float CallStatsAPI::getRoundTripDelay() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallStatsAPI::getRoundTripDelay", "this=" << this);
	return mCallStats->round_trip_delay;
}

//getSentRtcp() const;

int CallStatsAPI::getType() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallStatsAPI::getType", "this=" << this);
	return mCallStats->type;
}

float CallStatsAPI::getUploadBandwidth() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallStatsAPI::getUploadBandwidth", "this=" << this);
	return mCallStats->upload_bandwidth;
}

int CallStatsAPI::getUpnpState() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallStatsAPI::getUpnpState", "this=" << this);
	return mCallStats->upnp_state;
}

} // LinphoneWeb
