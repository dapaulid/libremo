//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "async.h"

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
//
// project
//
// C++
#include <future>
#include <mutex>
#include <vector>
//
// system
//
//------------------------------------------------------------------------------
namespace remo {
	namespace utils {
//------------------------------------------------------------------------------	

static std::vector<std::future<void>> s_pending_futures;
static std::mutex s_pending_futures_lock;

//------------------------------------------------------------------------------
// helpers
//------------------------------------------------------------------------------
//
static void fire_and_forget(std::future<void>&& a_future)
{
	std::lock_guard<std::mutex> lock(s_pending_futures_lock);

	// remove completed futures
	// TODO verify if correct
	for (size_t i = s_pending_futures.size(); i > 0; i--) {
		if (s_pending_futures[i-1].wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			s_pending_futures.erase(s_pending_futures.begin() + i-1);
		}
	}

	// TODO trace warning it too many in list?
	s_pending_futures.push_back(std::move(a_future));
}

//------------------------------------------------------------------------------
// functions
//------------------------------------------------------------------------------
//
void async(std::function<void()> a_func)
{
	// asynchronously call function without implicitly waiting on future 
	// see https://stackoverflow.com/questions/21531096/can-i-use-stdasync-without-waiting-for-the-future-limitation
    fire_and_forget(std::async(std::launch::async, a_func));
}

//------------------------------------------------------------------------------
	} // end namespace utils
} // end namespace remo
//------------------------------------------------------------------------------
