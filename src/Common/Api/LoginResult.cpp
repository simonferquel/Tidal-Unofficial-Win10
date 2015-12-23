#include "pch.h"
#include "LoginResult.h"

api::LoginResult::LoginResult(const web::json::value & json):
	userId(json.as_object().find(L"userId")->second.as_number().to_int64()),
	sessionId(json.as_object().find(L"sessionId")->second.as_string()),
	countryCode(json.as_object().find(L"countryCode")->second.as_string())
{
}
