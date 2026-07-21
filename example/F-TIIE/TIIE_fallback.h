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
#include <cassert>

#include <boost/decimal.hpp>

#include <calendar_algorithms.h>

#include <fixings.h>
#include <reset_math.h>
#include <scaled_value.h>



// or should we call it synthetic?
[[nodiscard]] inline auto fallback( // is this important enough to move to the main library?
	const reset::RateFixings& fix,
	const reset::RateFixings& target_rate_fix,
	const std::chrono::year_month_day& d, // do we assume it is always a good business day?
	const boost::decimal::decimal128_t& tenor
) -> reset::Percent // should it return reset::rate?
{
	using namespace boost::decimal::literals;

	const auto& calendar = fix.get_calendar();
	const auto prev = gregorian::shift_business_days(d, std::chrono::days{ -1 }, calendar);
	const auto prevprev = gregorian::shift_business_days(prev, std::chrono::days{ -1 }, calendar);

	const auto& _fixing = fix[prevprev];
	assert(_fixing);
	const auto fixing = static_cast<boost::decimal::decimal128_t>(*_fixing);

	const auto& _target_rate_prev_fixing = target_rate_fix[prev];
	assert(_target_rate_prev_fixing);
	const auto& _target_rate_prevprev_fixing = target_rate_fix[prevprev];
	assert(_target_rate_prevprev_fixing);
	const auto Banxico_move =
		static_cast<boost::decimal::decimal128_t>(*_target_rate_prev_fixing) -
		static_cast<boost::decimal::decimal128_t>(*_target_rate_prevprev_fixing);

	const auto _spread = reset::BasisPoints{ "24" }; // constexpr? // is this right that it is the same spread for all tenors?
	const auto spread = static_cast<boost::decimal::decimal128_t>(_spread);

	auto rate = (boost::decimal::pow(1_dl + (fixing + Banxico_move) / 360_dl, tenor) - 1_dl) * 360_dl / tenor; // should we use day count?
	rate = reset::round_dp(rate, 6u); // or should we be able to apply 4dp to the resulting percentage? (that would be closer to the documentation, which deals in percents)
	// should round_dp accept units for the power? (6dp or something like that)

	return rate + spread; // or do we need to apply the spread before rounding?
}
