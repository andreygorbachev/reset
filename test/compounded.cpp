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

#include <compounded.h>
#include <decimal.h>
#include <scaled_value.h>
#include <rate.h>
#include <fixings.h>

#include <gtest/gtest.h>

#include <actual_360.h>
#include <period.h>
#include <weekend.h>
#include <schedule.h>
#include <calendar.h>

#include <chrono>
#include <utility>

using namespace std;
using namespace std::chrono;
using namespace gregorian;
using namespace gregorian::util;
using namespace fin_calendar;
using namespace reset;


namespace reset
{

	TEST(compounded, compounded1)
	{
		// Create a tiny SOFR-like RateFixings around Good Friday 2026
		// Period: 2026-04-02 .. 2026-04-06
		auto ts = RateFixings::storage{ days_period{ 2026y / April / 2d, 2026y / April / 6d } };
		// Business days: 2026-04-02 (Thu) and 2026-04-06 (Mon)
		// Good Friday 2026-04-03 is a holiday (no fixing provided)
		ts[2026y / April / 2d] = Percent{ "1.80" };
		// 2026-04-03 left empty to model Good Friday (holiday)
		// weekend 4/4 and 4/5 also left empty
		ts[2026y / April / 6d] = Percent{ "1.75" };

		auto cal = calendar{ SaturdaySundayWeekend, schedule{ ts.get_period(), { 2026y / April / 3d } } };

		const auto fix = fixings{ std::move(ts), std::move(cal), 2u };

		auto rfd = rate_fixing_detail{};
		rfd.day_count = actual_360<Decimal>{};

		// Compound from 2026-04-02 (business) to 2026-04-06 (business) across Good Friday
		const auto rd = rate_detail{ 2026y / April / 2d, 2026y / April / 6d, rfd.day_count, 5u };
		const auto cd = compound_detail{ fix.get_calendar() };

		const auto result = compounded(fix, rfd, cd, rd);

		// For this case there is a single step using the 2026-04-02 fixing over 4 days.
		// The computed compounded annualised rate should equal the underlying daily rate (1.80%)
		EXPECT_EQ(Percent{ "1.80" }, result.percent);
	}

	TEST(compounded, compounded2)
	{
		// from the compounded index

		// test that we get an exception on a non business day (start/end)
		// (but maybe that is not correct and there is some fallback kicks in in this case)
	}

}
