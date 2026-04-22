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

#include "fixings.h"
#include "resets_math.h"
// do we need more includes for clarity?


namespace reset
{

	struct index_detail // should it be called metadata?
	{
		boost::multiprecision::cpp_dec_float_50 initial_value{ 1 };
		std::chrono::year_month_day initial_date{};
		bool brazil = false; // this needs to be better - maybe "calendar"/"business" compounding enum?
		std::optional<unsigned int> factor_trunc = std::nullopt;
		std::optional<unsigned int> factor_round = std::nullopt;
		std::optional<unsigned int> step_trunc = std::nullopt;
		std::optional<unsigned int> step_round = std::nullopt;
		std::optional<unsigned int> final_trunc = std::nullopt;
		std::optional<unsigned int> final_round = std::nullopt; // should rounding and truncations be int?
	};



	inline auto index_factor_(
		boost::multiprecision::cpp_dec_float_50 i,
		const std::chrono::year_month_day& start,
		const std::chrono::year_month_day& end,
		const fixings& fix,
		const rate_fixing_detail& rfd,
		const index_detail& detail
	) -> boost::multiprecision::cpp_dec_float_50
	{
		const auto rate = fix[start];

		const auto year_fraction = fin_calendar::fraction(start, end, rfd.day_count);

		const auto one = boost::multiprecision::cpp_dec_float_50{ 1 }; // constexpr would be better, but cpp_dec_float_50 does not support it
		auto factor = detail.brazil ?
			boost::multiprecision::cpp_dec_float_50{ pow(one + rate, year_fraction) } : // we are also missing rounding for Brazil year_fraction at the moment
			boost::multiprecision::cpp_dec_float_50{ one + rate * year_fraction }; // should these have some kind of units?

		if (detail.factor_trunc)
			factor = trunc_dp(factor, *detail.factor_trunc);

		if (detail.factor_round)
			factor = round_dp(factor, *detail.factor_round);

		return factor;
	}

	inline void index_step_(
		boost::multiprecision::cpp_dec_float_50& i,
		const std::chrono::year_month_day& start,
		const std::chrono::year_month_day& end,
		const fixings& fix,
		const rate_fixing_detail& rfd,
		const index_detail& id
	)
	{
		i *= index_factor_(i, start, end, fix, rfd, id);

		if (id.step_trunc)
			i = trunc_dp(i, *id.step_trunc);

		if (id.step_round)
			i = round_dp(i, *id.step_round);
	}


	// maybe this needs a better name? - compute a compounded RFR index from the underlying fixings
	inline auto index(
		const fixings& fix,
		const rate_fixing_detail& rfd,
		const std::chrono::year_month_day& ymd,
		const index_detail& id = index_detail{} // does it need a default?
	) -> boost::multiprecision::cpp_dec_float_50
	{
		// should throw an exception if we requested an index before a business day before the first reset
		// but we do not have information about relevant calendar at the moment

		const auto& c = fix.get_calendar();
		const auto schedule = c.make_business_days_schedule(
			gregorian::util::days_period{ id.initial_date, ymd }
		); // is this a wrong data structure?
		// assert that it is not empty?

		const auto& dates = schedule.get_dates();

		auto i = id.initial_value;

		// not very elegant to start with
		auto start = std::chrono::year_month_day{};
		for (const auto& d : dates)
		{
			if (d == *dates.cbegin())
			{
				start = d; // this probably means that we assume that the index starts on a business day - assert it?
				continue;
			}

			const auto& end = d;

			index_step_(i, start, end, fix, rfd, id);

			start = d;
		}

		if (id.final_trunc)
			i = trunc_dp(i, *id.final_trunc);

		if (id.final_round)
			i = round_dp(i, *id.final_round);

		return i;
	}

// we can also compute all the index values at the same time for all publication dates up to ymd
// adjusent_difference for year fraction
// partial_sum for daily compounding
// transform for final rounding

}
