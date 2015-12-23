#pragma once
#include <cpprest/json.h>
#include <chrono>
#include <date.h>
#include <string>
#include <sstream>
#include <iomanip>
#include "../tools/StringUtils.h"
namespace api {
	template<typename TFunc>
	bool doWithJsonFieldField(const web::json::value* obj, const std::wstring& field, TFunc&& cb) {
		if (!obj) {
			return false;
		}
		if (!obj->is_object()) {
			return false;
		}
		auto& o = obj->as_object();
		auto it = o.find(field);
		if (it == o.end()) {
			return false;
		}
		if (it->second.is_null()) {
			return false;
		}
		return cb(it->second);
	}

	template<typename T>
	bool parseJson(const web::json::value* obj, const std::wstring& field, T& result) {
		return doWithJsonFieldField(obj, field, [&result](const web::json::value& value) {
			if (value.is_object()) {
				result = T(value);
				return true;
			}
			return false;
		});
	}

	template<typename _Rep, typename _Period>
	inline bool parseJson(const web::json::value* obj, const std::wstring& field, std::chrono::duration<_Rep, _Period>& result) {
		return doWithJsonFieldField(obj, field, [&result](const web::json::value& value) {
			if (value.is_number()) {
				result = std::chrono::duration<_Rep, _Period>(value.as_number().to_int64());
				return true;
			}
			return false;
		});
	}
	inline bool parseJson(const web::json::value* obj, const std::wstring& field, std::chrono::system_clock::time_point& result) {
		using namespace std;
		using namespace std::chrono;
		using namespace date;
		return doWithJsonFieldField(obj, field, [&result](const web::json::value& value) {
			auto& str = value.as_string();
			int y, month, day, hour, minute, second, ms;
			wchar_t* _;
			if (str.size() >= 23) {
				y = wcstol(str.c_str(), &_, 10);
				month = wcstol(str.c_str() + 5, &_, 10);
				day = wcstol(str.c_str() + 8, &_, 10);
				hour = wcstol(str.c_str() + 11, &_, 10);
				minute = wcstol(str.c_str() + 14, &_, 10);
				second = wcstol(str.c_str() + 17, &_, 10);
				ms = wcstol(str.c_str() + 20, &_, 10);
				result = day_point{ year{ y } / month / day }+hours(hour) + minutes(minute) + seconds(second) + milliseconds(ms);
				return true;
			}
			return false;
		});
	}
	inline web::json::value toJson(std::chrono::system_clock::time_point tp) {
		using namespace std;
		using namespace std::chrono;
		using namespace date;
		auto dayPoint = floor<days>(tp);
		auto ymd = year_month_day{ dayPoint };
		auto h = floor<hours>(tp - dayPoint);
		auto m = floor<minutes>(tp - dayPoint - h);
		auto s = floor<seconds>(tp - dayPoint - h - m);
		auto ms = floor<milliseconds>(tp - dayPoint - h - m - s);
		wstringstream stream;
		stream << setfill(L'0') << setw(4) << (int)ymd.year();
		stream << L'-';
		stream << setfill(L'0') << setw(2) << (unsigned int)ymd.month();
		stream << L'-';
		stream << setfill(L'0') << setw(2) << (unsigned int)ymd.day();
		stream << L'T';
		stream << setfill(L'0') << setw(2) << h.count();
		stream << L':';
		stream << setfill(L'0') << setw(2) << m.count();
		stream << L':';
		stream << setfill(L'0') << setw(2) << s.count();
		stream << L'.';
		stream << setfill(L'0') << setw(3) << ms.count();
		return web::json::value::string(stream.str());
	}
	inline web::json::value toJson(const date::year_month_day& ymd) {
		using namespace std;
		using namespace std::chrono;
		using namespace date;
		
		wstringstream stream;
		stream << setfill(L'0') << setw(4) << (int)ymd.year();
		stream << L'-';
		stream << setfill(L'0') << setw(2) << (unsigned int)ymd.month();
		stream << L'-';
		stream << setfill(L'0') << setw(2) << (unsigned int)ymd.day();
		
		return web::json::value::string(stream.str());
	}
	inline bool parseJson(const web::json::value* obj, const std::wstring& field, date::year_month_day& result) {
		using namespace std;
		using namespace date;
		return doWithJsonFieldField(obj, field, [&result](const web::json::value& value) {
			auto& str = value.as_string();
			int y, month, day;
			wchar_t* _;
			if (str.size() >= 10) {
				y = wcstol(str.c_str(), &_, 10);
				month = wcstol(str.c_str() + 5, &_, 10);
				day = wcstol(str.c_str() + 8, &_, 10);
				
				result = year{ y } / month / day;
				return true;
			}
			return false;
		});
	}
	template<>
	inline bool parseJson(const web::json::value* obj, const std::wstring& field, std::wstring& result) {
	
		return doWithJsonFieldField(obj, field, [&result](const web::json::value& value) {
			if (value.is_string()) {
				result = value.as_string();
				return true;
			}
			return false;
		});
	}

	template<>
	inline bool parseJson(const web::json::value* obj, const std::wstring& field, Platform::String^& result) {

		return doWithJsonFieldField(obj, field, [&result](const web::json::value& value) {
			if (value.is_string()) {
				result = tools::strings::toWindowsString( value.as_string());
				return true;
			}
			return false;
		});
	}

	template<>
	inline bool parseJson(const web::json::value* obj, const std::wstring& field, bool& result) {
		return doWithJsonFieldField(obj, field, [&result](const web::json::value& value) {
			if (value.is_boolean()) {
				result = value.as_bool();
				return true;
			}
			return false;
		});
	}
	template<>
	inline bool parseJson(const web::json::value* obj, const std::wstring& field, std::int32_t& result) {
		return doWithJsonFieldField(obj, field, [&result](const web::json::value& value) {
			if (value.is_number()) {
				result = value.as_number().to_int32();
				return true;
			}
			return false;
		});
	}


	template<>
	inline bool parseJson(const web::json::value* obj, const std::wstring& field, std::int64_t& result) {
		return doWithJsonFieldField(obj, field, [&result](const web::json::value& value) {
			if (value.is_number()) {
				result = value.as_number().to_int64();
				return true;
			}
			return false;
		});
	}
	template<>
	inline bool parseJson(const web::json::value* obj, const std::wstring& field, double& result) {
		return doWithJsonFieldField(obj, field, [&result](const web::json::value& value) {
			if (value.is_number()) {
				result = value.as_number().to_double();
				return true;
			}
			return false;
		});
	}
	template<>
	inline bool parseJson(const web::json::value* obj, const std::wstring& field, const web::json::value*& result) {
		return doWithJsonFieldField(obj, field, [&result](const web::json::value& value) {
			result = &value;
			return true;
		});
	}

	template<>
	inline bool parseJson(const web::json::value* obj, const std::wstring& field, const web::json::array*& result) {
		return doWithJsonFieldField(obj, field, [&result](const web::json::value& value) {
			if (value.is_array()) {
				result = &value.as_array();
				return true;
			}
			return false;
		});
	}
}