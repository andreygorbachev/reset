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

#include <boost/multiprecision/cpp_dec_float.hpp>

#include <period.h>

#include <calendar.h>

#include <day_count.h>

#include "rate.h"
#include "fixings.h"
#include "resets_math.h"
// do we need more includes for clarity?


namespace reset
{

	struct average_detail // should it be called metadata?
	{
		std::chrono::days term{}; // for 30 day average, do we want it to be days{ 30 } or days{ -30 }?
		std::optional<unsigned int> final_round = std::nullopt; // should rounding and truncations be int?
	};



	inline void average_step_( // should it be the same as index_step_ in index.h?
		boost::multiprecision::cpp_dec_float_50& a, // should it take a return a value? (no in/out parameter)
		const std::chrono::year_month_day& start,
		const std::chrono::year_month_day& end,
		const fixings& fix,
		const rate_fixing_detail& rfd
	)
	{
//		const auto rate = fix[start];
		const auto rate = fix.current_observation(start); // or we can create special average_step_ for the first step when average starts on a non business day (and we need to use the previous reset)

		const auto year_fraction = fin_calendar::fraction(start, end, rfd.day_count);

		const auto one = boost::multiprecision::cpp_dec_float_50{ 1 }; // constexpr would be better, but cpp_dec_float_50 does not support it
		a *= boost::multiprecision::cpp_dec_float_50{ one + rate * year_fraction }; // should these have some kind of units?
	}

	// maybe this needs a better name? (like compounded 
	inline auto average(
		const fixings& fix,
		const rate_fixing_detail& rfd,
		const std::chrono::year_month_day& ymd,
		const average_detail& detail = average_detail{} // does it need a default?
	) -> rate
	{
		// do we handle the case where detail.term is empty?

		const auto& c = fix.get_calendar();
		const auto schedule = c.make_business_days_schedule(
			gregorian::util::days_period{ std::chrono::sys_days{ ymd } - detail.term, ymd}
		); // is this a wrong data structure?
		// assert that it is not empty?

		const auto& dates = schedule.get_dates();

		auto val = boost::multiprecision::cpp_dec_float_50{ 1 };

		// not very elegant to start with
		if (*dates.cbegin() == schedule.get_period().get_from())
		{
			auto start = std::chrono::year_month_day{};
			for (const auto& d : dates)
			{
				if (d == *dates.cbegin())
				{
					start = d;
					continue;
				}

				const auto& end = d;

				average_step_(val, start, end, fix, rfd);

				start = d;
			}
		}
		else
		{
			// special case where the first period starts on a non business day

			average_step_(val, schedule.get_period().get_from(), *dates.cbegin(), fix, rfd);

			auto start = std::chrono::year_month_day{};
			for (const auto& d : dates)
			{
				if (d == *dates.cbegin())
				{
					start = d;
					continue;
				}

				const auto& end = d;

				average_step_(val, start, end, fix, rfd);

				start = d;
			}
		}

		const auto year_fraction = fin_calendar::fraction(schedule.get_period(), rfd.day_count);

		const auto one = boost::multiprecision::cpp_dec_float_50{ 1 }; // constexpr would be better, but cpp_dec_float_50 does not support it
		auto a = boost::multiprecision::cpp_dec_float_50{ (val - one) / year_fraction };

		if (detail.final_round)
			a = round_dp(a, *detail.final_round);

		return {
			a,
			*dates.cbegin(),
			*dates.crbegin(),
			rfd.day_count // or should the average has its own day count? (is there a way to default it to underlying daily rate day count?)
		};
	}

}
