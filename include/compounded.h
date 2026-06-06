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

#include <day_count.h>

#include "rate.h"
#include "fixings.h"
#include "rate.h"
#include "index.h"


namespace reset
{

	// needs to handle lookback etc (also its own day count possibly)

	// maybe this needs a different name? (like compounded_rate)
	inline auto compounded(
		const RateFixings& fix,
		std::chrono::year_month_day start, // should it be a period instead?
		std::chrono::year_month_day end
	) -> rate
	{
		return {
			Percent{"0"}, // temp only
			start,
			end,
			fin_calendar::day_count<Decimal>{}
		};
	}


	inline auto compounded(
		const IndexFixings& fix,
		rate_detail detail
	) -> rate
	{
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

		auto rate = (end_fix->get_value() / start_fix->get_value() - Decimal{ 1 }) / year_fraction;

		rate = round_dp(rate, detail.round);

		return {
			std::move(rate),
			std::move(detail)
		};
	}

}
