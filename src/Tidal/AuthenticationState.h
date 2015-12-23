#pragma once
#include <cstdint>
#include <chrono>
#include <date.h>
#include "SoundQuality.h"
#include <cpprest/json.h>

namespace api {
	struct LoginResult;
	struct SubscribtionInfo;
	struct UserInfo;
}
class AuthenticationState
{
private:
	bool _isAuthenticated = false;
	std::int64_t _userId;
	Platform::String^ _sessionId;
	Platform::String^ _countryCode;

	std::chrono::system_clock::time_point _subscribtionValidUntil;
	Platform::String^ _subscribtionStatus;
	Platform::String^ _subscribtionType;
	date::days _subscribtionOfflineGracePeriod;
	SoundQuality _subscribtionHighestSoundQuality;
	bool _subscribtionPremiumAccess;
	bool _subscribtionCanGetTrial;

	Platform::String^ _userLogin;
	Platform::String^ _userFirstName;
	Platform::String^ _userLastName;
	Platform::String^ _userEmail;
	std::chrono::system_clock::time_point _userCreated;
	Platform::String^ _userPicture;
	bool _userNewsletter;
	Platform::String^ _userGender;
	date::year_month_day _userDateOfBirth;
	std::int64_t _userFacebookUid;
public:
	AuthenticationState();
	AuthenticationState(const api::LoginResult& loginResult, const api::SubscribtionInfo& subscribtionInfo, const api::UserInfo& userInfo);

	bool isAuthenticated()const { return _isAuthenticated; }
	std::int64_t userId()const { return _userId; }
	Platform::String^ sessionId()const { return _sessionId; }
	Platform::String^ countryCode()const { return _countryCode; }

	std::chrono::system_clock::time_point subscribtionValidUntil()const { return _subscribtionValidUntil; }
	Platform::String^ subscribtionStatus()const { return _subscribtionStatus; }
	Platform::String^ subscribtionType()const { return _subscribtionType; }
	date::days subscribtionOfflineGracePeriod()const { return _subscribtionOfflineGracePeriod; }
	SoundQuality subscribtionHighestSoundQuality()const { return _subscribtionHighestSoundQuality; }
	bool subscribtionPremiumAccess()const { return _subscribtionPremiumAccess; }
	bool subscribtionCanGetTrial()const { return _subscribtionCanGetTrial; }


	Platform::String^ userLogin()const { return _userLogin; }
	Platform::String^ userFirstName()const { return _userFirstName; }
	Platform::String^ userLastName()const { return _userLastName; }
	Platform::String^ userEmail()const { return _userEmail; }
	std::chrono::system_clock::time_point userCreated()const { return _userCreated; }
	Platform::String^ userPicture()const { return _userPicture; }
	bool userNewsletter()const { return _userNewsletter; }
	Platform::String^ userGender()const { return _userGender; }
	date::year_month_day userDateOfBirth()const { return _userDateOfBirth; }
	std::int64_t userFacebookUid()const { return _userFacebookUid; }

	web::json::value toJson()const;
	void loadJson(const web::json::value& json);
};

