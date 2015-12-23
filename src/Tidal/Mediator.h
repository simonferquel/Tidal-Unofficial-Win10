#pragma once
#include <functional>
#include <vector>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <ppltasks.h>

class RegistrationHolderBase {
protected:
	RegistrationHolderBase() = default;
public:
	virtual ~RegistrationHolderBase() {}
};
using RegistrationToken = std::unique_ptr<RegistrationHolderBase>;

template<typename ArgType>
class Mediator : public std::enable_shared_from_this<Mediator<ArgType>> {
public:
	using Callback = std::function<void(const ArgType&)>;


private:
	class RegistrationHolder : public RegistrationHolderBase {
	private:
		std::shared_ptr<Callback> _callback;
		std::shared_ptr<Mediator> _owner;
	public:
		RegistrationHolder(const std::shared_ptr<Callback>& callback, std::shared_ptr<Mediator>&& owner)
			: _callback(callback), _owner(std::move(owner))
		{}
		RegistrationHolder() = default;
		RegistrationHolder(const RegistrationHolder&) = delete;
		RegistrationHolder& operator=(const RegistrationHolder&) = delete;

		RegistrationHolder(RegistrationHolder&&) noexcept = default;
		RegistrationHolder& operator=(RegistrationHolder&&) noexcept = default;
		virtual ~RegistrationHolder() {
			if (_owner) {
				_owner->unregister(_callback);
			}
		}
	};
	std::shared_timed_mutex _mutex;
	std::vector<std::shared_ptr<Callback>> _callbacks;

	void unregister(const std::shared_ptr<Callback>& callback) {
		std::lock_guard<std::shared_timed_mutex> lg(_mutex);
		auto foundIt = std::find(_callbacks.cbegin(), _callbacks.cend(), callback);
		if (foundIt != _callbacks.cend()) {
			_callbacks.erase(foundIt);
		}
	}
public:

	template<typename TFunc>
	RegistrationToken registerCallback(TFunc&& callback) {
		auto callbackPtr = std::make_shared<Callback>(std::forward<TFunc>(callback));
		{
			std::lock_guard<std::shared_timed_mutex> lg(_mutex);
			_callbacks.push_back(callbackPtr);
		}
		return std::make_unique< RegistrationHolder>(callbackPtr, shared_from_this());
	}

	template <typename TObj>
	RegistrationToken registerCallback(TObj^ obj, void( __cdecl TObj::* func)(const ArgType&)) {
		Platform::WeakReference weakObj(obj);
		return registerCallback([weakObj, func](const ArgType& arg) {
			auto obj = weakObj.Resolve<TObj>();
			if (obj) {
				(obj->*func)(arg);
			}
		});

	}

	template <typename TObj>
	RegistrationToken registerCallbackNoArg(TObj^ obj, void(__cdecl TObj::* func)(void)) {
		Platform::WeakReference weakObj(obj);
		return registerCallback([weakObj, func](const ArgType& arg) {
			auto obj = weakObj.Resolve<TObj>();
			if (obj) {
				(obj->*func)();
			}
		});

	}

	void raise(const ArgType& arg) {
		std::shared_lock<std::shared_timed_mutex> lg(_mutex);
		for (auto& cb : _callbacks) {
			(*cb)(arg);
		}
	}
};


template<typename ArgType>
class AsyncMediator : public std::enable_shared_from_this<AsyncMediator<ArgType>> {
public:
	using Callback = std::function<concurrency::task<void>(const ArgType&)>;


private:
	class RegistrationHolder : public RegistrationHolderBase {
	private:
		std::shared_ptr<Callback> _callback;
		std::shared_ptr<AsyncMediator> _owner;
	public:
		RegistrationHolder(const std::shared_ptr<Callback>& callback, std::shared_ptr<AsyncMediator>&& owner)
			: _callback(callback), _owner(std::move(owner))
		{}
		RegistrationHolder() = default;
		RegistrationHolder(const RegistrationHolder&) = delete;
		RegistrationHolder& operator=(const RegistrationHolder&) = delete;

		RegistrationHolder(RegistrationHolder&&) noexcept = default;
		RegistrationHolder& operator=(RegistrationHolder&&) noexcept = default;
		virtual ~RegistrationHolder() {
			if (_owner) {
				_owner->unregister(_callback);
			}
		}
	};
	std::shared_timed_mutex _mutex;
	std::vector<std::shared_ptr<Callback>> _callbacks;

	void unregister(const std::shared_ptr<Callback>& callback) {
		std::lock_guard<std::shared_timed_mutex> lg(_mutex);
		auto foundIt = std::find(_callbacks.cbegin(), _callbacks.cend(), callback);
		if (foundIt != _callbacks.cend()) {
			_callbacks.erase(foundIt);
		}
	}
public:


	template<typename TFunc>
	RegistrationToken registerCallback(TFunc&& callback) {
		auto callbackPtr = std::make_shared<Callback>(std::forward<TFunc>(callback));
		{
			std::lock_guard<std::shared_timed_mutex> lg(_mutex);
			_callbacks.push_back(callbackPtr);
		}
		return std::make_unique< RegistrationHolder>(callbackPtr, shared_from_this());
	}

	template<typename TObj>
	RegistrationToken registerCallback(TObj^ obj, concurrency::task<void>(__cdecl TObj::* func)(const ArgType&)){
		Platform::WeakReference weakObj(obj);
		return registerCallback([weakObj, func](const ArgType& arg) {
			auto obj = weakObj.Resolve<TObj>();
			if (obj) {
				return (obj->*func)(arg);
			}
			else {
				return concurrency::task_from_result();
			}
		});

	}

	template<typename TObj>
	RegistrationToken registerCallbackNoArg(TObj^ obj, concurrency::task<void>(__cdecl TObj::* func)(void)) {
		Platform::WeakReference weakObj(obj);
		return registerCallback([weakObj, func](const ArgType& arg) {
			auto obj = weakObj.Resolve<TObj>();
			if (obj) {
				return (obj->*func)();
			}
			else {
				return concurrency::task_from_result();
			}
		});

	}

	concurrency::task<void> raise(const ArgType& arg) {
		std::shared_lock<std::shared_timed_mutex> lg(_mutex);
		std::vector<concurrency::task<void>> tasks;
		for (auto& cb : _callbacks) {
			tasks.push_back((*cb)(arg));
		}
		return concurrency::when_all(tasks.begin(), tasks.end());
	}
};
