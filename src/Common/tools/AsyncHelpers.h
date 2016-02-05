#pragma once
#include <ppltasks.h>
#include <functional>
namespace tools {
	namespace async {
		inline concurrency::task<void> WaitFor(Windows::Foundation::TimeSpan ts, concurrency::cancellation_token cancelToken) {
			concurrency::task_completion_event<void> tce;
			auto timer = Windows::System::Threading::ThreadPoolTimer::CreateTimer(ref new Windows::System::Threading::TimerElapsedHandler([tce](Windows::System::Threading::ThreadPoolTimer^ t) {
				tce.set();
			}), ts);
			if (cancelToken != concurrency::cancellation_token::none()) {
				auto tokenRegistration = cancelToken.register_callback([timer]() {
					timer->Cancel();
				});

				return concurrency::create_task(tce, cancelToken)
					.then([cancelToken, tokenRegistration](concurrency::task<void> t) {
					try {
						t.get();
						cancelToken.deregister_callback(tokenRegistration);
					}
					catch (...) {
						cancelToken.deregister_callback(tokenRegistration);
						throw;
					}
				});
			}
			else {
				return concurrency::create_task(tce);
			}
		}

		class TimeoutCancelTokenProvider {
		private:
			concurrency::cancellation_token_source _cts;
			Windows::System::Threading::ThreadPoolTimer^ _timer;
		public:
			TimeoutCancelTokenProvider(Windows::Foundation::TimeSpan ts) {
				auto cts = _cts;
				_timer = Windows::System::Threading::ThreadPoolTimer::CreateTimer(ref new Windows::System::Threading::TimerElapsedHandler([cts](Windows::System::Threading::ThreadPoolTimer^ t) {
					cts.cancel();
				}), ts);
			}
			~TimeoutCancelTokenProvider() {
				_timer->Cancel();
				_timer = nullptr;
			}

			concurrency::cancellation_token get_token()const {
				return _cts.get_token();
			}
		};

		concurrency::task<void> retryWithDelays(std::function<concurrency::task<void>(void)> action, Windows::Foundation::TimeSpan delay, concurrency::cancellation_token cancelToken);

		inline concurrency::cancellation_token combineCancelTokens(concurrency::cancellation_token first, concurrency::cancellation_token second) {
			if (!first.is_cancelable()) {
				return second;
			}
			else if (!second.is_cancelable()) {
				return first;
			}
			concurrency::cancellation_token tokens[] = { first, second };
			auto linkedCts = concurrency::cancellation_token_source::create_linked_source(tokens, tokens + ARRAYSIZE(tokens));
			return linkedCts.get_token();
		}

		

		template<typename T>
		struct cancellable_result {
			T result;
			bool cancelled;
		};

		template <>
		struct cancellable_result<void> {
			bool cancelled;
		};

		template<typename T>
		concurrency::task<cancellable_result<T>> swallowCancellationException(concurrency::task<T> t) {
			return t.then([](const concurrency::task<T>& t) {
				cancellable_result<T> result;
				try {
					result.result = t.get();
					result.cancelled = false;
				}
				catch (concurrency::task_canceled&) {
					result.cancelled = true;
				}
				return result;
			});
		}

		template<>
		inline concurrency::task<cancellable_result<void>> swallowCancellationException<void>(concurrency::task<void> t) {
			return t.then([](const concurrency::task<void>& t) {
				cancellable_result<void> result;
				try {
					t.get();
					result.cancelled = false;
				}
				catch (concurrency::task_canceled&) {
					result.cancelled = true;
				}
				return result;
			});
		}
	}
}