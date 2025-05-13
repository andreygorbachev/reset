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

#include "resets.h"


namespace reset
{

	// maybe this needs a better name? - compute a compounded RFR index from the underlying resets
	auto index(
		const resets& r,
		const std::chrono::year_month_day& ymd
		// for now assume 1 as a starting point and no rounding
	)
	{
		// should throw an exception if we requested an index before a business day before the first reset
		// but we do not have information about relevant calendar at the moment

		const auto& pc = r.get_publication_calendar();
		const auto& dc = r.get_day_count();

		const auto schedule = pc.make_business_days_schedule(
			gregorian::days_period{ r.get_time_series().get_period().get_from(), ymd }
		); // is this a wrong data structure?

		const auto i = resets::observation{ "1" };

		return i;
	}

}
