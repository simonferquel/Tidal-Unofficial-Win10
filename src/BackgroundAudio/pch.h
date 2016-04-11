//
// pch.h
// Header for standard system include files.
//

#pragma once

#include <collection.h>
#include <ppltasks.h>
#include <ppl.h>
#include <experimental/coroutine>
using namespace Platform;
#include <pplawait.h>

#undef min
#undef max

//
//namespace std
//{
//	namespace experimental
//	{
//		template <typename... _Whatever>
//		struct coroutine_traits<void, _Whatever...>
//		{
//			struct promise_type
//			{
//				void get_return_object() const
//				{
//				}
//
//				bool initial_suspend() const { return (false); }
//
//				bool final_suspend() const { return (false); }
//
//				void return_void(void)
//				{
//				}
//
//				void set_exception(const exception_ptr &_Ptr)
//				{
//					std::rethrow_exception(_Ptr);
//				}
//			};
//		};
//	}
//}