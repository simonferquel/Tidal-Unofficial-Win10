#pragma once
enum class SoundQuality {
	Unknown = 0,
	Low = 1,
	High = 2,
	Lossless = 3,
	LosslessHd = 4
};

inline bool operator < (SoundQuality lhs, SoundQuality rhs) {
	return static_cast<int>(lhs) < static_cast<int>(rhs);
}
inline bool operator <= (SoundQuality lhs, SoundQuality rhs) {
	return static_cast<int>(lhs) <= static_cast<int>(rhs);
}
inline bool operator > (SoundQuality lhs, SoundQuality rhs) {
	return static_cast<int>(lhs) > static_cast<int>(rhs);
}
inline bool operator >= (SoundQuality lhs, SoundQuality rhs) {
	return static_cast<int>(lhs) >= static_cast<int>(rhs);
}

inline SoundQuality parseSoundQuality(Platform::String^ txt) {
	if (txt == L"LOSSLESS_HD") {
		return SoundQuality::LosslessHd;
	}
	if (txt == L"LOSSLESS") {
		return SoundQuality::Lossless;
	}
	if (txt == L"HIGH") {
		return SoundQuality::High;
	}
	if (txt == L"LOW") {
		return SoundQuality::Low;
	}
	return SoundQuality::Unknown;
}
inline Platform::String^ toString(SoundQuality quality) {
	switch (quality)
	{
	case SoundQuality::Low:
		return L"LOW";
	case SoundQuality::High:
		return L"HIGH";
	case SoundQuality::Lossless:
		return L"LOSSLESS";
	case SoundQuality::LosslessHd:
		return L"LOSSLESS_HD";
	default:
		return L"UNKNOWN";
	}
}