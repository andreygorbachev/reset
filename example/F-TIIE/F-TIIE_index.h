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

#include <fixings.h>
#include <index.h>
#include <reset_math.h>
#include <scaled_value.h>

#include <preceding.h>



[[nodiscard]] inline auto non_business_day_index( // is this important enough to move to the main library?
	const reset::RateFixings& fix,
	const reset::rate_fixings_detail& rfd,
	const std::chrono::year_month_day& ymd,
	const reset::index_detail& id = reset::index_detail{} // does it need a default?
) -> reset::Value
{
	if (fix.get_calendar().is_business_day(ymd))
		return reset::index(fix, rfd, ymd, id);
	else
	{
		constexpr auto preceding = fin_calendar::preceding{};
		const auto prev = preceding.adjust(ymd, fix.get_calendar());

		auto indx = reset::index(fix, rfd, prev, id).get_value();
		reset::index_step_(indx, prev, ymd, fix, rfd, id);

		if (id.final_trunc)
			indx = reset::trunc_dp(indx, *id.final_trunc);

		if (id.final_round)
			indx = reset::round_dp(indx, *id.final_round);

		return indx;
	}
}
