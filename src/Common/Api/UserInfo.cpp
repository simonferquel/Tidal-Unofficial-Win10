#include "pch.h"
#include "UserInfo.h"
#include "JsonHelpers.h"
using namespace date;

api::UserInfo::UserInfo(const web::json::value & json): dateOfBirth(1970_y/1/1)
{
	parseJson(&json, L"id", id);
	parseJson(&json, L"username", username);
	parseJson(&json, L"firstName", firstName);
	parseJson(&json, L"lastName", lastName);
	parseJson(&json, L"email", email);
	parseJson(&json, L"created", created);
	parseJson(&json, L"picture", picture);
	parseJson(&json, L"newsletter", newsletter);
	parseJson(&json, L"gender", gender);
	parseJson(&json, L"dateOfBirth", dateOfBirth);
	parseJson(&json, L"facebookUid", facebookUid);
}
