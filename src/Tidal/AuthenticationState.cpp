#include "pch.h"
#include "AuthenticationState.h"
#include "Api/UserInfo.h"
#include "Api/SubscribtionInfo.h"
#include "Api/LoginResult.h"
#include "tools/StringUtils.h"
#include "Api/JsonHelpers.h"
using namespace tools::strings;
using namespace date;
AuthenticationState::AuthenticationState() :
	_userDateOfBirth(1970_y/1/1)
{
}

AuthenticationState::AuthenticationState(const api::LoginResult& loginResult, const api::SubscribtionInfo& subscribtionInfo, const api::UserInfo& userInfo):
	_isAuthenticated(true),
	_userId(loginResult.userId),
	_sessionId(toWindowsString(loginResult.sessionId)),
	_countryCode(toWindowsString(loginResult.countryCode)),
	_subscribtionValidUntil(subscribtionInfo.validUntil),
	_subscribtionType(toWindowsString(subscribtionInfo.type)),
	_subscribtionStatus(toWindowsString(subscribtionInfo.status)),
	_subscribtionOfflineGracePeriod(subscribtionInfo.offlineGracePeriod),
	_subscribtionHighestSoundQuality(parseSoundQuality(toWindowsString(subscribtionInfo.highestSoundQuality))),
	_subscribtionPremiumAccess(subscribtionInfo.premiumAccess),
	_subscribtionCanGetTrial(subscribtionInfo.canGetTrial),
	_userLogin(toWindowsString(userInfo.username)),
	_userFirstName(toWindowsString(userInfo.firstName)),
	_userLastName(toWindowsString(userInfo.lastName)),
	_userEmail(toWindowsString(userInfo.email)),
	_userCreated(userInfo.created),
	_userPicture(toWindowsString(userInfo.picture)),
	_userNewsletter(userInfo.newsletter),
	_userGender(toWindowsString(userInfo.gender)),
	_userDateOfBirth(userInfo.dateOfBirth),
	_userFacebookUid(userInfo.facebookUid)
{
}

web::json::value AuthenticationState::toJson() const
{
	std::vector<std::pair<std::wstring, web::json::value>> fields;
	fields.emplace_back(L"isAuthenticated", web::json::value::boolean(_isAuthenticated));
	fields.emplace_back(L"userId", web::json::value::number(_userId));
	fields.emplace_back(L"sessionId", web::json::value::string(toStdString(_sessionId)));
	fields.emplace_back(L"countryCode", web::json::value::string(toStdString(_countryCode)));
	fields.emplace_back(L"subscribtionValidUntil", api::toJson(_subscribtionValidUntil));
	fields.emplace_back(L"subscribtionStatus", web::json::value::string(toStdString(_subscribtionStatus)));
	fields.emplace_back(L"subscribtionType", web::json::value::string(toStdString(_subscribtionType)));
	fields.emplace_back(L"subscribtionOfflineGracePeriod", web::json::value::number(_subscribtionOfflineGracePeriod.count()));
	fields.emplace_back(L"subscribtionHighestSoundQuality", web::json::value::string(toStdString(toString(_subscribtionHighestSoundQuality))));
	fields.emplace_back(L"subscribtionPremiumAccess", web::json::value::boolean(_subscribtionPremiumAccess));
	fields.emplace_back(L"subscribtionCanGetTrial", web::json::value::boolean(_subscribtionCanGetTrial));
	fields.emplace_back(L"userLogin", web::json::value::string(toStdString(_userLogin)));
	fields.emplace_back(L"userFirstName", web::json::value::string(toStdString(_userFirstName)));
	fields.emplace_back(L"userLastName", web::json::value::string(toStdString(_userLastName)));
	fields.emplace_back(L"userEmail", web::json::value::string(toStdString(_userEmail)));
	fields.emplace_back(L"userCreated", api::toJson(_userCreated));
	fields.emplace_back(L"userPicture", web::json::value::string(toStdString(_userPicture)));
	fields.emplace_back(L"userNewsletter", web::json::value::boolean(_userNewsletter));
	fields.emplace_back(L"userGender", web::json::value::string(toStdString(_userGender)));
	fields.emplace_back(L"userDateOfBirth", api::toJson(_userDateOfBirth));
	fields.emplace_back(L"userFacebookUid", web::json::value::number(_userFacebookUid));
	return web::json::value::object(fields);
}

void AuthenticationState::loadJson(const web::json::value & json)
{
	using namespace api;
	parseJson(&json, L"isAuthenticated", _isAuthenticated);
	parseJson(&json, L"userId", _userId);
	parseJson(&json, L"sessionId", _sessionId);
	parseJson(&json, L"countryCode", _countryCode);
	parseJson(&json, L"subscribtionValidUntil", _subscribtionValidUntil);
	parseJson(&json, L"subscribtionStatus", _subscribtionStatus);
	parseJson(&json, L"subscribtionType", _subscribtionType);
	parseJson(&json, L"subscribtionOfflineGracePeriod", _subscribtionOfflineGracePeriod);
	Platform::String^ soundQualityStr;
	parseJson(&json, L"subscribtionHighestSoundQuality", soundQualityStr);
	_subscribtionHighestSoundQuality = parseSoundQuality(soundQualityStr);
	parseJson(&json, L"subscribtionPremiumAccess", _subscribtionPremiumAccess);
	parseJson(&json, L"subscribtionCanGetTrial", _subscribtionCanGetTrial);
	parseJson(&json, L"userLogin", _userLogin);
	parseJson(&json, L"userFirstName", _userFirstName);
	parseJson(&json, L"userLastName", _userLastName);
	parseJson(&json, L"userEmail", _userEmail);
	parseJson(&json, L"userCreated", _userCreated);
	parseJson(&json, L"userPicture", _userPicture);
	parseJson(&json, L"userNewsletter", _userNewsletter);
	parseJson(&json, L"userGender", _userGender);
	parseJson(&json, L"userDateOfBirth", _userDateOfBirth);
	parseJson(&json, L"userFacebookUid", _userFacebookUid);
}


