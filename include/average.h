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
#include <optional>

#include <period.h>

#include <calendar.h>

#include <day_count.h>

#include "decimal.h"
#include "rate.h"
#include "fixings.h"
#include "reset_math.h"


namespace reset
{

	struct average_detail // should it be called metadata?
	{
		std::chrono::days term{}; // for 30 day average, do we want it to be days{ 30 } or days{ -30 }?
		std::optional<unsigned int> final_round = std::nullopt; // should rounding and truncation be int?
	};


	inline void average_step_(
		Decimal& a,
		const std::chrono::year_month_day& start,
		const std::chrono::year_month_day& end,
		const RateFixings& fix,
		const rate_fixing_detail& rfd
	);


	// maybe this needs a better name? (like compounded average)
	inline auto average(
		const RateFixings& fix,
		const rate_fixing_detail& rfd,
		const std::chrono::year_month_day& ymd,
		const average_detail& detail = average_detail{} // does it need a default?
	) -> rate
	{
		// do we handle the case where detail.term is empty?

		const auto average_start = std::chrono::sys_days{ ymd } - detail.term;
		const auto average_end = ymd; // I think we assume that ymd is a good business day - should we check for that?

		const auto& c = fix.get_calendar();
		const auto schedule = c.make_business_days_schedule(
			gregorian::util::days_period{ average_start, average_end }
		); // is this a wrong data structure?
		// assert that it is not empty?

		auto dates = schedule.get_dates(); // we might consider something not making a copy as most use cases would not need to insert
		if (!dates.contains(average_start)) // or we can just have a look at cbegin(), which is O(1) operation on most platforms, rather than O(log n)
			dates.insert(average_start); // do it with hint?

		auto val = Decimal{ 1 };

		for (const auto& [start, end] : dates | std::views::adjacent<2uz>)
			average_step_(val, start, end, fix, rfd);

		const auto year_fraction = fin_calendar::fraction(schedule.get_period(), rfd.day_count);

		const auto one = Decimal{ 1 }; // constexpr would be better, but cpp_dec_float_50 does not support it
		auto a = Decimal{ (val - one) / year_fraction };

		if (detail.final_round)
			a = round_dp(a, *detail.final_round);

		return {
			a,
			average_start,
			average_end,
			rfd.day_count // or should the average has its own day count? (is there a way to default it to underlying daily rate day count?)
		};
	}


	inline void average_step_( // should it be the same as index_step_ in index.h?
		Decimal& a, // should it take and return a value? (no in/out parameter)
		const std::chrono::year_month_day& start,
		const std::chrono::year_month_day& end,
		const RateFixings& fix,
		const rate_fixing_detail& rfd
	)
	{
		const auto& fixing = fix.with_fallback(start); // or we can create special average_step_ for the first step when average starts on a non business day (and we need to use the previous reset)
		const auto rate = static_cast<Decimal>(fixing);

		const auto year_fraction = fin_calendar::fraction(start, end, rfd.day_count);

		const auto one = Decimal{ 1 }; // constexpr would be better, but cpp_dec_float_50 does not support it
		a *= Decimal{ one + rate * year_fraction }; // should these have some kind of units?
	}

}
