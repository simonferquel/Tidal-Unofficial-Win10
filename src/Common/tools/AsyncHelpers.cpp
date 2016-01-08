#include "pch.h"
#include "AsyncHelpers.h"
#include <memory>
using namespace concurrency;


struct AsyncRetryWithDelaysStateMachine : public std::enable_shared_from_this<AsyncRetryWithDelaysStateMachine> {
	task_completion_event<void> _tce;
	std::function<concurrency::task<void>(void)> _action;
	Windows::Foundation::TimeSpan _delay;
	concurrency::cancellation_token _cancelToken;
	AsyncRetryWithDelaysStateMachine(std::function<concurrency::task<void>(void)>&& action, Windows::Foundation::TimeSpan delay, concurrency::cancellation_token cancelToken) :
		_action(std::move(action)),
		_delay(delay),
		_cancelToken(cancelToken)
	{

	}
	void tryAndWait() {
		_action()
			.then([that = shared_from_this()](task<void> t){
			try {
				t.get();
				that->_tce.set();
			}
			catch (...) {
				if (that->_cancelToken.is_canceled()) {
					that->_tce.set_exception(task_canceled());
				}
				else {
					tools::async::WaitFor(that->_delay, that->_cancelToken)
						.then([that](concurrency::task<void> t) {
						try {
							t.get();
							that->tryAndWait();
						}
						catch (task_canceled&) {
							that->_tce.set_exception(std::current_exception());
						}
					});
				}
			}
		});
	}
	task<void> start() {
		tryAndWait();
		return create_task(_tce);
	}
};

concurrency::task<void> tools::async::retryWithDelays(std::function<concurrency::task<void>(void)> action, Windows::Foundation::TimeSpan delay, concurrency::cancellation_token cancelToken)
{
	auto stateMachine = std::make_shared<AsyncRetryWithDelaysStateMachine>(std::move(action), delay, cancelToken);
	return stateMachine->start();
}
