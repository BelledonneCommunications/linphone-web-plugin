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

#ifndef H_PRESENCEAPI
#define H_PRESENCEAPI

#include <linphone/linphonecore.h>
#include <linphone/linphonepresence.h>
#include "wrapperapi.h"

namespace LinphoneWeb {

FB_FORWARD_PTR(PresenceActivityAPI)
FB_FORWARD_PTR(PresenceNoteAPI)
FB_FORWARD_PTR(PresencePersonAPI)
FB_FORWARD_PTR(PresenceServiceAPI)

class PresenceActivityAPI: public WrapperAPI {
	friend class FactoryAPI;

private:
	LinphonePresenceActivity *mActivity;

	PresenceActivityAPI(LinphonePresenceActivity *activity);
	PresenceActivityAPI(int type, StringPtr const &description);

protected:
	virtual void initProxy();

public:
	virtual ~PresenceActivityAPI();

	StringPtr toString() const;
	void setType(int type);
	int getType() const;
	void setDescription(StringPtr const &description);
	StringPtr getDescription() const;

	inline LinphonePresenceActivity *getRef() {
		return mActivity;
	}
};

class PresenceModelAPI: public WrapperAPI {
	friend class FactoryAPI;

private:
	LinphonePresenceModel *mModel;

	PresenceModelAPI();
	PresenceModelAPI(LinphonePresenceModel *model);
	PresenceModelAPI(const LinphonePresenceModel *model);
	PresenceModelAPI(int acttype, StringPtr const &description);
	PresenceModelAPI(int acttype, StringPtr const &description, StringPtr const &note, StringPtr const &lang);

protected:
	virtual void initProxy();

public:
	virtual ~PresenceModelAPI();

	int getBasicStatus() const;
	void setBasicStatus(int basicStatus);
	time_t getTimestamp() const;
	StringPtr getContact() const;
	void setContact(StringPtr const &contact);
	PresenceActivityAPIPtr getActivity() const;
	int setActivity(int acttype, StringPtr const &description);
	int getNbActivities() const;
	PresenceActivityAPIPtr getNthActivity(int idx) const;
	int addActivity(PresenceActivityAPIPtr const &activity);
	int clearActivities();
	PresenceNoteAPIPtr getNote(StringPtr const &lang) const;
	int addNote(StringPtr const &content, StringPtr const &lang);
	int clearNotes();

	int getNbServices() const;
	PresenceServiceAPIPtr getNthService(int idx) const;
	int addService(PresenceServiceAPIPtr const &service);
	int clearServices();
	int getNbPersons() const;
	PresencePersonAPIPtr getNthPerson(int idx) const;
	int addPerson(PresencePersonAPIPtr const &person);
	int clearPersons();

	inline LinphonePresenceModel *getRef() {
		return mModel;
	}
};

class PresenceNoteAPI: public WrapperAPI {
	friend class FactoryAPI;

private:
	LinphonePresenceNote *mNote;

	PresenceNoteAPI(LinphonePresenceNote *note);
	PresenceNoteAPI(StringPtr const &content, StringPtr const &lang);

protected:
	virtual void initProxy();

public:
	virtual ~PresenceNoteAPI();

	StringPtr getContent() const;
	void setContent(StringPtr const &content);
	StringPtr getLang() const;
	void setLang(StringPtr const &lang);

	inline LinphonePresenceNote *getRef() {
		return mNote;
	}
};

class PresencePersonAPI: public WrapperAPI {
	friend class FactoryAPI;

private:
	LinphonePresencePerson *mPerson;

	PresencePersonAPI(LinphonePresencePerson *person);
	PresencePersonAPI(StringPtr const &id);

protected:
	virtual void initProxy();

public:
	virtual ~PresencePersonAPI();

	StringPtr getId() const;
	void setId(StringPtr const &id);
	int getNbActivities() const;
	PresenceActivityAPIPtr getNthActivity(int idx) const;
	int addActivity(PresenceActivityAPIPtr const &activity);
	int clearActivities();
	int getNbNotes() const;
	PresenceNoteAPIPtr getNthNote(int idx) const;
	int addNote(PresenceNoteAPIPtr const &note);
	int clearNotes();
	int getNbActivitiesNotes() const;
	PresenceNoteAPIPtr getNthActivitiesNote(int idx) const;
	int addActivitiesNote(PresenceNoteAPIPtr const &note);
	int clearActivitiesNotes();

	inline LinphonePresencePerson *getRef() {
		return mPerson;
	}
};

class PresenceServiceAPI: public WrapperAPI {
	friend class FactoryAPI;

private:
	LinphonePresenceService *mService;

	PresenceServiceAPI(LinphonePresenceService *service);
	PresenceServiceAPI(StringPtr const &id, int basicStatus, StringPtr const &contact);

protected:
	virtual void initProxy();

public:
	virtual ~PresenceServiceAPI();

	void setId(StringPtr const &id);
	StringPtr getId() const;
	void setBasicStatus(int basicStatus);
	int getBasicStatus() const;
	void setContact(StringPtr const &contact);
	StringPtr getContact() const;
	int getNbNotes() const;
	PresenceNoteAPIPtr getNthNote(int idx) const;
	int addNote(PresenceNoteAPIPtr const &note);
	int clearNotes();

	inline LinphonePresenceService *getRef() {
		return mService;
	}
};

} // LinphoneWeb
	
#endif //H_PRESENCEAPI
