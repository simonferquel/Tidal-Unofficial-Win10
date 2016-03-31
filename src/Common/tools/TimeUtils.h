#pragma once
#include <chrono>
#include <cstdint>
#include <Windows.h>
#include <string>
#include <date.h>
namespace tools {
	namespace time {
		using nanox100 = std::ratio<1, 10000000>;
		using WindowsTimeSpanChronoType = std::chrono::duration<std::int64_t, nanox100>;

		inline WindowsTimeSpanChronoType WindowsToChrono(const Windows::Foundation::TimeSpan& ts) {
			return WindowsTimeSpanChronoType(ts.Duration);
		}

		inline std::chrono::system_clock::time_point WindowsToChrono(const Windows::Foundation::DateTime& ts) {
			using namespace std::chrono;
			SYSTEMTIME st;
			FILETIME ft;
			ULARGE_INTEGER val;
			val.QuadPart = ts.UniversalTime;
			ft.dwHighDateTime = val.HighPart;
			ft.dwLowDateTime = val.LowPart;

			FileTimeToSystemTime(&ft, &st);
			return date::day_point{ date::year(st.wYear) / st.wMonth / st.wDay }
				+ hours(st.wHour)
				+ minutes(st.wMinute)
				+ seconds(st.wSecond)
				+ milliseconds(st.wMilliseconds);
		}

		inline Windows::Foundation::TimeSpan ChronoTypeToWindowsTimeSpan(const WindowsTimeSpanChronoType& chrono) {
			Windows::Foundation::TimeSpan result;
			result.Duration = chrono.count();
			return result;
		}

		inline Windows::Foundation::DateTime ChronoTypeToWindowsDateTime(const std::chrono::system_clock::time_point& chrono) {
			using namespace std::chrono;
			using namespace date;
			auto windowsEpoch = day_point{ 1601_y / 1 / 1 };
			WindowsTimeSpanChronoType timeSinceWinEpoch = chrono - windowsEpoch;
			Windows::Foundation::DateTime result;
			result.UniversalTime = timeSinceWinEpoch.count();
			return result;
		}

		template<typename TDuration>
		TDuration WindowsTo(const Windows::Foundation::TimeSpan& ts) {
			return std::chrono::duration_cast<TDuration>(WindowsToChrono(ts));
		}

		template<typename TDuration>
		Windows::Foundation::TimeSpan ToWindowsTimeSpan(const TDuration& ts) {
			return (ChronoTypeToWindowsTimeSpan(std::chrono::duration_cast<WindowsTimeSpanChronoType>(ts)));
		}

		inline std::int64_t getCurrentUnixTimeStamp() {
			using namespace std::chrono;
			using namespace date;
			auto unixEpoch = day_point{ 1970_y / 1 / 1 };
			return date::floor<seconds>(system_clock::now() - unixEpoch).count();
		}
		inline std::wstring toStringMMSS(const std::chrono::seconds& totalSecs) {
			auto hours = totalSecs.count() / 3600;
			auto mins = (totalSecs.count() / 60)%60;
			auto secs = totalSecs.count() % 60;
			std::wstring result;
			if (hours > 0) {
				result.append(std::to_wstring(hours));
				result.push_back(L':');
			}
			if (mins < 10) {
				result.push_back(L'0');
			}
			result.append(std::to_wstring(mins));
			result.push_back(L':');

			if (secs < 10) {
				result.push_back(L'0');
			}
			result.append(std::to_wstring(secs));

			return result;
		}
		inline std::wstring toString(const std::chrono::seconds& totalSecs) {
			auto hours = totalSecs.count() / 3600;
			auto mins = (totalSecs.count() / 60) % 60;
			auto secs = totalSecs.count() % 60;
			std::wstring result;
			if (hours > 0) {
				result.append(std::to_wstring(hours));
				result.push_back(L'h');
			}
			if (mins < 10) {
				result.push_back(L'0');
			}
			result.append(std::to_wstring(mins));
			result.push_back(L'm');
			if (hours == 0) {
				if (secs < 10) {
					result.push_back(L'0');
				}
				result.append(std::to_wstring(secs));
				result.push_back(L's');
			}
			return result;
		}
	}
}