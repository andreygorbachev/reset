// The MIT License (MIT)
//
// Copyright (c) 2023 Andrey Gorbachev
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <chrono>
#include <utility>
#include <stdexcept>

#include <boost/decimal.hpp>

#include <day_count.h>

#include <period.h>
#include <calendar.h>

#include "rate.h"
#include "fixings.h"
#include "reset_math.h"


namespace reset // maybe should be in a separate lib (as it uses resets, so one level up) // what about other constructs in this project
{

	struct compounded_detail
	{

		// does it need its own start/end? (at the moment these are lifted from rate_detail, but maybe they need to be separate?)

		// needs to handle lookback etc
	
		gregorian::calendar calendar; // does it need to be a copy?

	};


	inline void compounded_step_(
		boost::decimal::decimal128_t& val,
		const std::chrono::year_month_day& start,
		const std::chrono::year_month_day& end,
		const RateFixings& fix,
		const rate_fixings_detail& rfd
	);


	// maybe this needs a different name? (like compounded_rate)
	inline auto compounded(
		const RateFixings& fix,
		const rate_fixings_detail& rfd,
		const compounded_detail& cd,
		rate_detail rd
	) -> rate
	{
		using namespace boost::decimal::literals;

		const auto schedule = cd.calendar.make_business_days_schedule(
			gregorian::util::days_period{ rd.start, rd.end }
		); // is this a wrong data structure?
		// assert that it is not empty?

		auto dates = schedule.get_dates(); // we might consider something not making a copy as most use cases would not need to insert
		if (!dates.contains(rd.start)) // or we can just have a look at cbegin(), which is O(1) operation on most platforms, rather than O(log n)
			dates.insert(rd.start); // do it with hint?

		auto val = 1_DL;

		for (const auto& [start, end] : dates | std::views::adjacent<2uz>)
			compounded_step_(val, start, end, fix, rfd);

		const auto year_fraction = fin_calendar::fraction(
			rd.start,
			rd.end,
			rd.day_count
		);

		auto rate = (val - 1_DL) / year_fraction;

		rate = round_dp(rate, rd.round);

		return {
			std::move(rate),
			std::move(rd)
		};
	}


	inline auto compounded(
		const IndexFixings& fix,
		rate_detail detail
	) -> rate
	{
		using namespace boost::decimal::literals;

		const auto& start_fix = fix[detail.start];
		if (!start_fix)
			throw std::runtime_error{ "No fixing for start date" }; // is this a correct exception type?
		const auto& end_fix = fix[detail.end];
		if (!end_fix)
			throw std::runtime_error{ "No fixing for end date" }; // is this a correct exception type?

		const auto year_fraction = fin_calendar::fraction(
			detail.start,
			detail.end,
			detail.day_count
		);

		auto rate = (end_fix->get_value() / start_fix->get_value() - 1_DL) / year_fraction;

		rate = round_dp(rate, detail.round);

		return {
			std::move(rate),
			std::move(detail)
		};
	}


	inline void compounded_step_( // should it be the same as index_step_ in index.h?
		boost::decimal::decimal128_t& val, // should it take and return a value? (no in/out parameter)
		const std::chrono::year_month_day& start,
		const std::chrono::year_month_day& end,
		const RateFixings& fix,
		const rate_fixings_detail& rfd
	)
	{
		using namespace boost::decimal::literals;

		const auto& fixing = fix.with_fallback(start); // I guess this is just a "number" and the "details" are provided separately - is it good?
		const auto rate = static_cast<boost::decimal::decimal128_t>(fixing);

		const auto year_fraction = fin_calendar::fraction(start, end, rfd.day_count);

		val *= 1_DL + rate * year_fraction; // should these have some kind of units?
	}

}
