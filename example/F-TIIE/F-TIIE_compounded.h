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

#include <rate.h>
#include <fixings.h>
#include <reset_math.h>
#include <scaled_value.h>



[[nodiscard]] inline auto compounded_in_advance( // is this important enough to move to the main library?
	const reset::IndexFixings& fix,
	const std::chrono::year_month_day& d,
	const boost::decimal::decimal128_t& tenor
) -> reset::Percent // should it return reset::rate?
{
	using namespace boost::decimal::literals;

	const auto& _index_d = fix[d];
	assert(_index_d); // we assume that requests are only made for business days, but actually index is given for all calendar days
	const auto index_d = static_cast<boost::decimal::decimal128_t>(*_index_d);

	const auto d_28n = std::chrono::sys_days{ d } - std::chrono::days{ 28 };
	const auto& _index_d_28n = fix[d_28n];
	assert(_index_d_28n);
	const auto index_d_28n = static_cast<boost::decimal::decimal128_t>(*_index_d_28n);

	auto rate = (boost::decimal::pow(index_d / index_d_28n, tenor / 28_dl) - 1_dl) * 360_dl / tenor; // should we use day count?
	rate = reset::round_dp(rate, 6u); // or should we be able to apply 4dp to the resulting percentage? (that would be closer to the documentation, which deals in percents)
	// should round_dp accept units for the power? (6dp or something like that)

	return reset::Percent{ rate };
}
