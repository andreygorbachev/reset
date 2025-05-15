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

#include <calendar.h>

#include <day_count.h>

#include "resets.h"
#include "resets_math.h"
// do we need more includes for clarity?


namespace reset
{

	struct index_detail
	{
		boost::multiprecision::cpp_dec_float_50 initial_value{ "1" };
		std::chrono::year_month_day initial_date{};
		std::optional<unsigned int> factor_trunc = std::nullopt;
		std::optional<unsigned int> factor_round = std::nullopt;
		std::optional<unsigned int> step_trunc = std::nullopt;
		std::optional<unsigned int> step_round = std::nullopt;
		std::optional<unsigned int> final_trunc = std::nullopt;
		std::optional<unsigned int> final_round = std::nullopt;
	};

	template<typename T>
	auto index_step_(
		T i,
		const std::chrono::year_month_day& start,
		const std::chrono::year_month_day& end,
		const resets& r,
		const index_detail& detail
	) -> T
	{
		const auto rate = r[start];

		const auto& dc = r.get_day_count();

		const auto year_fraction = fin_calendar::fraction(start, end, dc);

		const auto one = T{ "1" };
//		auto factor = T{ one + rate * year_fraction }; // should these have some kind of units?
		auto factor = T{ pow(one + rate, year_fraction) }; // should these have some kind of units?

		if (detail.factor_trunc)
			factor = trunc_dp(factor, *detail.factor_trunc);

		if (detail.factor_round)
			factor = round_dp(factor, *detail.factor_round);

		i *= factor; // factor out one more substep?

		if (detail.step_trunc)
			i = trunc_dp(i, *detail.step_trunc);

		if (detail.step_round)
			i = round_dp(i, *detail.step_round);

		return i;
	}


	// maybe this needs a better name? - compute a compounded RFR index from the underlying resets
	auto index(
		const resets& r,
		const std::chrono::year_month_day& ymd,
		const index_detail& detail = index_detail{}
	)
	{
		// should throw an exception if we requested an index before a business day before the first reset
		// but we do not have information about relevant calendar at the moment

		const auto& c = r.get_calendar();
		const auto schedule = c.make_business_days_schedule(
			gregorian::days_period{ detail.initial_date, ymd }
		); // is this a wrong data structure?
		// assert that it is not empty?

		const auto& dates = schedule.get_dates();

		auto i = detail.initial_value;

		// not very elegant to start with
		auto start = std::chrono::year_month_day{};
		for (const auto& d : dates)
		{
			if (d == *dates.cbegin())
			{
				start = d;
				continue;
			}

			const auto& end = d;

			i = index_step_(i, start, end, r, detail);

			start = d;
		}

		if (detail.final_trunc)
			i = trunc_dp(i, *detail.final_trunc);

		if(detail.final_round)
			i = round_dp(i, *detail.final_round);

		return i;
	}

}
