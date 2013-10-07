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

#include "presenceapi.h"

#include "utils.h"
#include "factoryapi.h"

namespace LinphoneWeb {

PresenceActivityAPI::PresenceActivityAPI(LinphonePresenceActivity *activity) :
		WrapperAPI(APIDescription(this)), mActivity(activity) {
	FBLOG_DEBUG("PresenceActivityAPI::PresenceActivityAPI", "this=" << this << "\t" << "activity=" << activity);
	linphone_presence_activity_ref(mActivity);
	linphone_presence_activity_set_user_data(mActivity, this);
}

PresenceActivityAPI::PresenceActivityAPI(int type, StringPtr const &description) :
		WrapperAPI(APIDescription(this)) {
	FBLOG_DEBUG("PresenceActivityAPI::PresenceActivityAPI", "this=" << this << "\t" << "type=" << type << "\t" << "description=" << description);
	mActivity = linphone_presence_activity_new((LinphonePresenceActivityType)type, STRING_TO_CHARPTR(description));
	if (mActivity == NULL) {
		throw std::invalid_argument("type and/or description is/are invalid");
	}
	linphone_presence_activity_set_user_data(mActivity, this);
}

void PresenceActivityAPI::initProxy() {
	registerProperty("description", make_property(this, &PresenceActivityAPI::getDescription, &PresenceActivityAPI::setDescription));
	registerProperty("type", make_property(this, &PresenceActivityAPI::getType, &PresenceActivityAPI::setType));

	registerMethod("toString", make_method(this, &PresenceActivityAPI::toString));
}

PresenceActivityAPI::~PresenceActivityAPI() {
	FBLOG_DEBUG("PresenceActivityAPI::~PresenceActivityAPI", "this=" << this);
	if (mActivity != NULL) {
		linphone_presence_activity_set_user_data(mActivity, NULL);
		linphone_presence_activity_unref(mActivity);
	}
}

StringPtr PresenceActivityAPI::toString() const {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceActivityAPI::toString", "this=" << this);
	return CHARPTR_TO_STRING(linphone_presence_activity_to_string(mActivity));
}

void PresenceActivityAPI::setType(int type) {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceActivityAPI::setType", "this=" << this << "\t" << "type=" << type);
	linphone_presence_activity_set_type(mActivity, (LinphonePresenceActivityType)type);
}

int PresenceActivityAPI::getType() const {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceActivityAPI::getType", "this=" << this);
	return linphone_presence_activity_get_type(mActivity);
}

void PresenceActivityAPI::setDescription(StringPtr const &description) {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceActivityAPI::setDescription", "this=" << this << "\t" << "description=" << description);
	linphone_presence_activity_set_description(mActivity, STRING_TO_CHARPTR(description));
}

StringPtr PresenceActivityAPI::getDescription() const {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceActivityAPI::getDescription", "this=" << this);
	return CHARPTR_TO_STRING(linphone_presence_activity_get_description(mActivity));
}



PresenceModelAPI::PresenceModelAPI() :
		WrapperAPI(APIDescription(this)) {
	FBLOG_DEBUG("PresenceModelAPI::PresenceModelAPI", "this=" << this);
	mModel = linphone_presence_model_new();
	linphone_presence_model_set_user_data(mModel, this);
}

PresenceModelAPI::PresenceModelAPI(LinphonePresenceModel *model) :
		WrapperAPI(APIDescription(this)), mModel(model) {
	FBLOG_DEBUG("PresenceModelAPI::PresenceModelAPI", "this=" << this << "\t" << "model=" << model);
	linphone_presence_model_ref(mModel);
	linphone_presence_model_set_user_data(mModel, this);
}

PresenceModelAPI::PresenceModelAPI(const LinphonePresenceModel *model) :
		WrapperAPI(APIDescription(this)), mModel(const_cast<LinphonePresenceModel *>(model)) {
	FBLOG_DEBUG("PresenceModelAPI::PresenceModelAPI", "this=" << this << "\t" << "model=" << model);
	linphone_presence_model_ref(mModel);
	linphone_presence_model_set_user_data(mModel, this);
}

PresenceModelAPI::PresenceModelAPI(int acttype, StringPtr const &description) :
		WrapperAPI(APIDescription(this)) {
	FBLOG_DEBUG("PresenceModelAPI::PresenceModelAPI", "this=" << this << "\t" << "acttype=" << acttype << "\t" << "description=" << description);
	mModel = linphone_presence_model_new_with_activity((LinphonePresenceActivityType)acttype, STRING_TO_CHARPTR(description));
	linphone_presence_model_set_user_data(mModel, this);
}

PresenceModelAPI::PresenceModelAPI(int acttype, StringPtr const &description, StringPtr const &note, StringPtr const &lang) :
		WrapperAPI(APIDescription(this)) {
	FBLOG_DEBUG("PresenceModelAPI::PresenceModelAPI", "this=" << this << "\t" << "acttype=" << acttype << "\t" << "description=" << description << "\t" << "note=" << note << "\t" << "lang=" << lang);
	mModel = linphone_presence_model_new_with_activity_and_note((LinphonePresenceActivityType)acttype, STRING_TO_CHARPTR(description), STRING_TO_CHARPTR(note), STRING_TO_CHARPTR(lang));
	linphone_presence_model_set_user_data(mModel, this);
}

void PresenceModelAPI::initProxy() {
	registerProperty("timestamp", make_property(this, &PresenceModelAPI::getTimestamp));
	registerProperty("nbActivities", make_property(this, &PresenceModelAPI::getNbActivities));
	registerProperty("nbPersons", make_property(this, &PresenceModelAPI::getNbPersons));
	registerProperty("nbServices", make_property(this, &PresenceModelAPI::getNbServices));
	registerProperty("activity", make_property(this, &PresenceModelAPI::getActivity));

	if (isConst()) {
		registerProperty("basicStatus", make_property(this, &PresenceModelAPI::getBasicStatus));
		registerProperty("contact", make_property(this, &PresenceModelAPI::getContact));
	} else {
		registerProperty("basicStatus", make_property(this, &PresenceModelAPI::getBasicStatus, &PresenceModelAPI::setBasicStatus));
		registerProperty("contact", make_property(this, &PresenceModelAPI::getContact, &PresenceModelAPI::setContact));
	}

	registerMethod("getNote", make_method(this, &PresenceModelAPI::getNote));
	registerMethod("getNthActivity", make_method(this, &PresenceModelAPI::getNthActivity));
	registerMethod("getNthPerson", make_method(this, &PresenceModelAPI::getNthPerson));
	registerMethod("getNthService", make_method(this, &PresenceModelAPI::getNthService));

	if (!isConst()) {
		registerMethod("addActivity", make_method(this, &PresenceModelAPI::addActivity));
		registerMethod("addNote", make_method(this, &PresenceModelAPI::addNote));
		registerMethod("addPerson", make_method(this, &PresenceModelAPI::addPerson));
		registerMethod("addService", make_method(this, &PresenceModelAPI::addService));
		registerMethod("clearActivities", make_method(this, &PresenceModelAPI::clearActivities));
		registerMethod("clearNotes", make_method(this, &PresenceModelAPI::clearNotes));
		registerMethod("clearPersons", make_method(this, &PresenceModelAPI::clearPersons));
		registerMethod("clearServices", make_method(this, &PresenceModelAPI::clearServices));
		registerMethod("setActivity", make_method(this, &PresenceModelAPI::setActivity));
	}
}

PresenceModelAPI::~PresenceModelAPI() {
	FBLOG_DEBUG("PresenceModelAPI::~PresenceModelAPI", "this=" << this);
	if (mModel != NULL) {
		linphone_presence_model_set_user_data(mModel, NULL);
		linphone_presence_model_unref(mModel);
	}
}

void PresenceModelAPI::setBasicStatus(int basicStatus) {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::setBasicStatus", "this=" << this << "\t" << "basicStatus=" << basicStatus);
	linphone_presence_model_set_basic_status(mModel, (LinphonePresenceBasicStatus)basicStatus);
}

int PresenceModelAPI::getBasicStatus() const {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::getBasicStatus", "this=" << this);
	return linphone_presence_model_get_basic_status(mModel);
}

time_t PresenceModelAPI::getTimestamp() const {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::getTimestamp", "this=" << this);
	// WARNING: Keep only the least-significant 32 bits because of difference of definition of time_t between mingw and visual studio
	return (linphone_presence_model_get_timestamp(mModel) & 0xFFFFFFFF);
}

void PresenceModelAPI::setContact(StringPtr const &contact) {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::setContact", "this=" << this << "\t" << "contact=" << contact);
	linphone_presence_model_set_contact(mModel, STRING_TO_CHARPTR(contact));
}

StringPtr PresenceModelAPI::getContact() const {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::getContact", "this=" << this);
	return CHARPTR_TO_STRING(linphone_presence_model_get_contact(mModel));
}

PresenceActivityAPIPtr PresenceModelAPI::getActivity() const {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::getActivity", "this=" << this);
	LinphonePresenceActivity *activity = linphone_presence_model_get_activity(mModel);
	return getFactory()->getPresenceActivity(activity);
}

int PresenceModelAPI::setActivity(int acttype, StringPtr const &description) {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::setActivity", "this=" << this << "\t" << "acttype=" << acttype << "\t" << "description=" << description);
	return linphone_presence_model_set_activity(mModel, (LinphonePresenceActivityType)acttype, STRING_TO_CHARPTR(description));
}

int PresenceModelAPI::getNbActivities() const {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::getNbActivities", "this=" << this);
	return linphone_presence_model_get_nb_activities(mModel);
}

PresenceActivityAPIPtr PresenceModelAPI::getNthActivity(int idx) const {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::getNthActivity", "this=" << this << "\t" << "idx=" << idx);
	LinphonePresenceActivity *activity = linphone_presence_model_get_nth_activity(mModel, idx);
	return getFactory()->getPresenceActivity(activity);
}

int PresenceModelAPI::addActivity(PresenceActivityAPIPtr const &activity) {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::addActivity", "this=" << this << "\t" << "activity=" << activity);
	return linphone_presence_model_add_activity(mModel, activity->getRef());
}

int PresenceModelAPI::clearActivities() {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::clearActivities", "this=" << this);
	return linphone_presence_model_clear_activities(mModel);
}

PresenceNoteAPIPtr PresenceModelAPI::getNote(StringPtr const &lang) const {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::getNote", "this=" << this << "\t" << "lang=" << lang);
	LinphonePresenceNote *note = linphone_presence_model_get_note(mModel, STRING_TO_CHARPTR(lang));
	return getFactory()->getPresenceNote(note);
}

int PresenceModelAPI::addNote(StringPtr const &content, StringPtr const &lang) {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::addNote", "this=" << this << "\t" << "content=" << content << "\t" << "lang=" << lang);
	return linphone_presence_model_add_note(mModel, STRING_TO_CHARPTR(content), STRING_TO_CHARPTR(lang));
}

int PresenceModelAPI::clearNotes() {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::clearNotes", "this=" << this);
	return linphone_presence_model_clear_notes(mModel);
}

int PresenceModelAPI::getNbServices() const {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::getNbServices", "this=" << this);
	return linphone_presence_model_get_nb_services(mModel);
}

PresenceServiceAPIPtr PresenceModelAPI::getNthService(int idx) const {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::getNthService", "this=" << this << "\t" << "idx=" << idx);
	LinphonePresenceService *service = linphone_presence_model_get_nth_service(mModel, idx);
	return getFactory()->getPresenceService(service);
}

int PresenceModelAPI::addService(PresenceServiceAPIPtr const &service) {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::addService", "this=" << this << "\t" << "service=" << service);
	return linphone_presence_model_add_service(mModel, service->getRef());
}

int PresenceModelAPI::clearServices() {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::clearServices", "this=" << this);
	return linphone_presence_model_clear_services(mModel);
}

int PresenceModelAPI::getNbPersons() const {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::getNbPersons", "this=" << this);
	return linphone_presence_model_get_nb_persons(mModel);
}

PresencePersonAPIPtr PresenceModelAPI::getNthPerson(int idx) const {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::getNthPerson", "this=" << this << "\t" << "idx=" << idx);
	LinphonePresencePerson *person = linphone_presence_model_get_nth_person(mModel, idx);
	return getFactory()->getPresencePerson(person);
}

int PresenceModelAPI::addPerson(PresencePersonAPIPtr const &person) {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::addPerson", "this=" << this << "\t" << "person=" << person);
	return linphone_presence_model_add_person(mModel, person->getRef());
}

int PresenceModelAPI::clearPersons() {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceModelAPI::clearPersons", "this=" << this);
	return linphone_presence_model_clear_persons(mModel);
}



PresenceNoteAPI::PresenceNoteAPI(LinphonePresenceNote *note) :
		WrapperAPI(APIDescription(this)), mNote(note) {
	FBLOG_DEBUG("PresenceNoteAPI::PresenceNoteAPI", "this=" << this << "\t" << "note=" << note);
	linphone_presence_note_ref(mNote);
	linphone_presence_note_set_user_data(mNote, this);
}

PresenceNoteAPI::PresenceNoteAPI(StringPtr const &content, StringPtr const &lang) :
		WrapperAPI(APIDescription(this)) {
	FBLOG_DEBUG("PresenceNoteAPI::PresenceNoteAPI", "this=" << this << "\t" << "content=" << content << "\t" << "lang=" << lang);
	mNote = linphone_presence_note_new(STRING_TO_CHARPTR(content), STRING_TO_CHARPTR(lang));
	if (mNote == NULL) {
		throw std::invalid_argument("content and/or lang is/are invalid");
	}
	linphone_presence_note_set_user_data(mNote, this);
}

void PresenceNoteAPI::initProxy() {
	registerProperty("content", make_property(this, &PresenceNoteAPI::getContent, &PresenceNoteAPI::setContent));
	registerProperty("lang", make_property(this, &PresenceNoteAPI::getLang, &PresenceNoteAPI::setLang));
}

PresenceNoteAPI::~PresenceNoteAPI() {
	FBLOG_DEBUG("PresenceNoteAPI::~PresenceNoteAPI", "this=" << this);
	if (mNote != NULL) {
		linphone_presence_note_set_user_data(mNote, NULL);
		linphone_presence_note_unref(mNote);
	}
}

StringPtr PresenceNoteAPI::getContent() const {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceNoteAPI::getContent", "this=" << this);
	return CHARPTR_TO_STRING(linphone_presence_note_get_content(mNote));
}

void PresenceNoteAPI::setContent(StringPtr const &content) {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceNoteAPI::setContent", "this=" << this << "\t" << "content=" << content);
	linphone_presence_note_set_content(mNote, STRING_TO_CHARPTR(content));
}

StringPtr PresenceNoteAPI::getLang() const {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceNoteAPI::getLang", "this=" << this);
	return CHARPTR_TO_STRING(linphone_presence_note_get_lang(mNote));
}

void PresenceNoteAPI::setLang(StringPtr const &lang) {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceNoteAPI::setLang", "this=" << this << "\t" << "lang=" << lang);
	linphone_presence_note_set_lang(mNote, STRING_TO_CHARPTR(lang));
}



PresencePersonAPI::PresencePersonAPI(LinphonePresencePerson *person) :
		WrapperAPI(APIDescription(this)), mPerson(person) {
	FBLOG_DEBUG("PresencePersonAPI::PresencePersonAPI", "this=" << this << "\t" << "person=" << person);
	linphone_presence_person_ref(mPerson);
	linphone_presence_person_set_user_data(mPerson, this);
}

PresencePersonAPI::PresencePersonAPI(StringPtr const &id) :
		WrapperAPI(APIDescription(this)) {
	FBLOG_DEBUG("PresencePersonAPI::PresencePersonAPI", "this=" << this << "\t" << "id=" << id);
	mPerson = linphone_presence_person_new(STRING_TO_CHARPTR(id));
	if (mPerson == NULL) {
		throw std::invalid_argument("id is invalid");
	}
	linphone_presence_person_set_user_data(mPerson, this);
}

void PresencePersonAPI::initProxy() {
	registerProperty("id", make_property(this, &PresencePersonAPI::getId, &PresencePersonAPI::setId));
	registerProperty("nbActivities", make_property(this, &PresencePersonAPI::getNbActivities));
	registerProperty("nbActivitiesNotes", make_property(this, &PresencePersonAPI::getNbActivitiesNotes));
	registerProperty("nbNotes", make_property(this, &PresencePersonAPI::getNbNotes));

	registerMethod("addActivity", make_method(this, &PresencePersonAPI::addActivity));
	registerMethod("addActivitiesNote", make_method(this, &PresencePersonAPI::addActivitiesNote));
	registerMethod("addNote", make_method(this, &PresencePersonAPI::addNote));
	registerMethod("clearActivities", make_method(this, &PresencePersonAPI::clearActivities));
	registerMethod("clearActivitiesNotes", make_method(this, &PresencePersonAPI::clearActivitiesNotes));
	registerMethod("clearNotes", make_method(this, &PresencePersonAPI::clearNotes));
	registerMethod("getNthActivity", make_method(this, &PresencePersonAPI::getNthActivity));
	registerMethod("getNthActivitiesNote", make_method(this, &PresencePersonAPI::getNthActivitiesNote));
	registerMethod("getNthNote", make_method(this, &PresencePersonAPI::getNthNote));
}

PresencePersonAPI::~PresencePersonAPI() {
	FBLOG_DEBUG("PresencePersonAPI::~PresencePersonAPI", "this=" << this);
	if (mPerson != NULL) {
		linphone_presence_person_set_user_data(mPerson, NULL);
		linphone_presence_person_unref(mPerson);
	}
}

void PresencePersonAPI::setId(StringPtr const &id) {
	CORE_MUTEX

	FBLOG_DEBUG("PresencePersonAPI::setId", "this=" << this << "\t" << "id=" << id);
	linphone_presence_person_set_id(mPerson, STRING_TO_CHARPTR(id));
}

StringPtr PresencePersonAPI::getId() const {
	CORE_MUTEX

	FBLOG_DEBUG("PresencePersonAPI::getId", "this=" << this);
	return CHARPTR_TO_STRING(linphone_presence_person_get_id(mPerson));
}

int PresencePersonAPI::getNbActivities() const {
	CORE_MUTEX

	FBLOG_DEBUG("PresencePersonAPI::getNbActivities", "this=" << this);
	return linphone_presence_person_get_nb_activities(mPerson);
}

PresenceActivityAPIPtr PresencePersonAPI::getNthActivity(int idx) const {
	CORE_MUTEX

	FBLOG_DEBUG("PresencePersonAPI::getNthActivity", "this=" << this << "\t" << "idx=" << idx);
	LinphonePresenceActivity *activity = linphone_presence_person_get_nth_activity(mPerson, idx);
	return getFactory()->getPresenceActivity(activity);
}

int PresencePersonAPI::addActivity(PresenceActivityAPIPtr const &activity) {
	CORE_MUTEX

	FBLOG_DEBUG("PresencePersonAPI::addActivity", "this=" << this << "\t" << "activity=" << activity);
	return linphone_presence_person_add_activity(mPerson, activity->getRef());
}

int PresencePersonAPI::clearActivities() {
	CORE_MUTEX

	FBLOG_DEBUG("PresencePersonAPI::clearActivities", "this=" << this);
	return linphone_presence_person_clear_activities(mPerson);
}

int PresencePersonAPI::getNbNotes() const {
	CORE_MUTEX

	FBLOG_DEBUG("PresencePersonAPI::getNbNotes", "this=" << this);
	return linphone_presence_person_get_nb_notes(mPerson);
}

PresenceNoteAPIPtr PresencePersonAPI::getNthNote(int idx) const {
	CORE_MUTEX

	FBLOG_DEBUG("PresencePersonAPI::getNthNote", "this=" << this << "\t" << "idx=" << idx);
	LinphonePresenceNote *note = linphone_presence_person_get_nth_note(mPerson, idx);
	return getFactory()->getPresenceNote(note);
}

int PresencePersonAPI::addNote(PresenceNoteAPIPtr const &note) {
	CORE_MUTEX

	FBLOG_DEBUG("PresencePersonAPI::addNote", "this=" << this << "\t" << "note=" << note);
	return linphone_presence_person_add_note(mPerson, note->getRef());
}

int PresencePersonAPI::clearNotes() {
	CORE_MUTEX

	FBLOG_DEBUG("PresencePersonAPI::clearNotes", "this=" << this);
	return linphone_presence_person_clear_notes(mPerson);
}

int PresencePersonAPI::getNbActivitiesNotes() const {
	CORE_MUTEX

	FBLOG_DEBUG("PresencePersonAPI::getNbActivitiesNotes", "this=" << this);
	return linphone_presence_person_get_nb_activities_notes(mPerson);
}

PresenceNoteAPIPtr PresencePersonAPI::getNthActivitiesNote(int idx) const {
	CORE_MUTEX

	FBLOG_DEBUG("PresencePersonAPI::getNthActivitiesNote", "this=" << this << "\t" << "idx=" << idx);
	LinphonePresenceNote *note = linphone_presence_person_get_nth_activities_note(mPerson, idx);
	return getFactory()->getPresenceNote(note);
}

int PresencePersonAPI::addActivitiesNote(PresenceNoteAPIPtr const &note) {
	CORE_MUTEX

	FBLOG_DEBUG("PresencePersonAPI::addActivitiesNote", "this=" << this << "\t" << "note=" << note);
	return linphone_presence_person_add_activities_note(mPerson, note->getRef());
}

int PresencePersonAPI::clearActivitiesNotes() {
	CORE_MUTEX

	FBLOG_DEBUG("PresencePersonAPI::clearActivitiesNotes", "this=" << this);
	return linphone_presence_person_clear_activities_notes(mPerson);
}



PresenceServiceAPI::PresenceServiceAPI(LinphonePresenceService *service) :
		WrapperAPI(APIDescription(this)), mService(service) {
	FBLOG_DEBUG("PresenceServiceAPI::PresenceServiceAPI", "this=" << this << "\t" << "service=" << service);
	linphone_presence_service_ref(mService);
	linphone_presence_service_set_user_data(mService, this);
}

PresenceServiceAPI::PresenceServiceAPI(StringPtr const &id, int basicStatus, StringPtr const &contact) :
		WrapperAPI(APIDescription(this)) {
	FBLOG_DEBUG("PresenceServiceAPI::PresenceServiceAPI", "this=" << this << "\t" << "id=" << id << "\t" << "basicStatus=" << basicStatus << "\t" << "contact=" << contact);
	mService = linphone_presence_service_new(STRING_TO_CHARPTR(id), (LinphonePresenceBasicStatus)basicStatus, STRING_TO_CHARPTR(contact));
	if (mService == NULL) {
		throw std::invalid_argument("id and/or basicStatus and/or contact is/are invalid");
	}
	linphone_presence_service_set_user_data(mService, this);
}

void PresenceServiceAPI::initProxy() {
	registerProperty("basicStatus", make_property(this, &PresenceServiceAPI::getBasicStatus, &PresenceServiceAPI::setBasicStatus));
	registerProperty("contact", make_property(this, &PresenceServiceAPI::getContact, &PresenceServiceAPI::setContact));
	registerProperty("id", make_property(this, &PresenceServiceAPI::getId, &PresenceServiceAPI::setId));
	registerProperty("nbNotes", make_property(this, &PresenceServiceAPI::getNbNotes));

	registerMethod("addNote", make_method(this, &PresenceServiceAPI::addNote));
	registerMethod("clearNotes", make_method(this, &PresenceServiceAPI::clearNotes));
	registerMethod("getNthNote", make_method(this, &PresenceServiceAPI::getNthNote));
}

PresenceServiceAPI::~PresenceServiceAPI() {
	FBLOG_DEBUG("PresenceServiceAPI::~PresenceServiceAPI", "this=" << this);
	if (mService != NULL) {
		linphone_presence_service_set_user_data(mService, NULL);
		linphone_presence_service_unref(mService);
	}
}

void PresenceServiceAPI::setId(StringPtr const &id) {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceServiceAPI::setId", "this=" << this << "\t" << "id=" << id);
	linphone_presence_service_set_id(mService, STRING_TO_CHARPTR(id));
}

StringPtr PresenceServiceAPI::getId() const {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceServiceAPI::getId", "this=" << this);
	return CHARPTR_TO_STRING(linphone_presence_service_get_id(mService));
}

void PresenceServiceAPI::setBasicStatus(int basicStatus) {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceServiceAPI::setBasicStatus", "this=" << this << "\t" << "basicStatus=" << basicStatus);
	linphone_presence_service_set_basic_status(mService, (LinphonePresenceBasicStatus)basicStatus);
}

int PresenceServiceAPI::getBasicStatus() const {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceServiceAPI::getBasicStatus", "this=" << this);
	return linphone_presence_service_get_basic_status(mService);
}

void PresenceServiceAPI::setContact(StringPtr const &contact) {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceServiceAPI::setContact", "this=" << this << "\t" << "contact=" << contact);
	linphone_presence_service_set_contact(mService, STRING_TO_CHARPTR(contact));
}

StringPtr PresenceServiceAPI::getContact() const {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceServiceAPI::getContact", "this=" << this);
	return CHARPTR_TO_STRING(linphone_presence_service_get_contact(mService));
}

int PresenceServiceAPI::getNbNotes() const {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceServiceAPI::getNbNotes", "this=" << this);
	return linphone_presence_service_get_nb_notes(mService);
}

PresenceNoteAPIPtr PresenceServiceAPI::getNthNote(int idx) const {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceServiceAPI::getNthNote", "this=" << this << "\t" << "idx=" << idx);
	LinphonePresenceNote *note = linphone_presence_service_get_nth_note(mService, idx);
	return getFactory()->getPresenceNote(note);
}

int PresenceServiceAPI::addNote(PresenceNoteAPIPtr const &note) {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceServiceAPI::addNote", "this=" << this << "\t" << "note=" << note);
	return linphone_presence_service_add_note(mService, note->getRef());
}

int PresenceServiceAPI::clearNotes() {
	CORE_MUTEX

	FBLOG_DEBUG("PresenceServiceAPI::clearNotes", "this=" << this);
	return linphone_presence_service_clear_notes(mService);
}

} // LinphoneWeb
