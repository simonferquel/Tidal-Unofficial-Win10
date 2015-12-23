#pragma once
#include <memory>
#include <vector>
namespace tools {
	class ScopedEventRegistrationBase {
	private:
		Windows::Foundation::EventRegistrationToken _registrationToken;
		bool _moved = false;
	protected:
		bool moved()const { return _moved; }
		Windows::Foundation::EventRegistrationToken registrationToken() const {
			return _registrationToken;
		}
		ScopedEventRegistrationBase(const Windows::Foundation::EventRegistrationToken& token) : _registrationToken(token) {

		}
	public:
		ScopedEventRegistrationBase() : _moved(true) {

		}
		ScopedEventRegistrationBase(const ScopedEventRegistrationBase&) = delete;
		ScopedEventRegistrationBase(ScopedEventRegistrationBase&& moved) {
			_registrationToken = moved._registrationToken;
			moved._moved = true;
		}
		ScopedEventRegistrationBase& operator =(const ScopedEventRegistrationBase&) = delete;
		ScopedEventRegistrationBase& operator =(ScopedEventRegistrationBase&&) = delete;

		virtual ~ScopedEventRegistrationBase() {
		}
	};

	template<typename TRemoveHandlerCallback>
	class ScopedEventRegistration : public ScopedEventRegistrationBase {
	private:
		std::remove_reference_t<std::remove_cv_t<TRemoveHandlerCallback>> _callback;
	protected:
	public:
		ScopedEventRegistration() = default;
		ScopedEventRegistration(const Windows::Foundation::EventRegistrationToken& token, TRemoveHandlerCallback&& callback) :
			ScopedEventRegistrationBase(token),
			_callback(std::forward<TRemoveHandlerCallback>(callback))
		{

		}

		virtual ~ScopedEventRegistration() {
			if (!moved()) {

				_callback(registrationToken());
			}
		}

	};

	template<typename TRemoveHandlerCallback>
	std::unique_ptr<ScopedEventRegistrationBase> makeScopedEventRegistration(const Windows::Foundation::EventRegistrationToken& token, TRemoveHandlerCallback&& callback) {
		return std::make_unique<ScopedEventRegistration<TRemoveHandlerCallback>>(token, std::forward<TRemoveHandlerCallback>(callback));
	}


	using ScopedEventRegistrations = std::vector<std::unique_ptr<ScopedEventRegistrationBase>>;

}