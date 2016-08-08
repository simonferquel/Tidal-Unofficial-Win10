#pragma once

template <typename T>
struct Hat {
	Hat(T^ p) {
		::Platform::Object^ __obj = p;
		p_ = reinterpret_cast<__abi_IUnknown*>(__obj);
		if (p_) {
			p_->__abi_AddRef();
		}
	}
	~Hat() { if (p_) { p_->__abi_Release(); } }
	Hat(Hat const&) = delete;
	Hat(Hat&& other) : p_(other.p_) { other.p_ = nullptr; }
	T^ operator->() const { return static_cast<T^>(reinterpret_cast<::Platform::Object^>(p_)); }
	T^ release() {
		__abi_IUnknown* tmp = p_;
		p_ = nullptr;
		return static_cast<T^>(reinterpret_cast<::Platform::Object^>(tmp));
	}
	T^ get() const {
		return static_cast<T^>(reinterpret_cast<::Platform::Object^>(p_));
	}
private:
	__abi_IUnknown* p_;
};
