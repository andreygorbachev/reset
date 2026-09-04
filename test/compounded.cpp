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
#include <scaled_value.h>
#include <rate.h>
#include <fixings.h>

#include <gtest/gtest.h>

#include <actual_360.h>
#include <period.h>
#include <weekend.h>
#include <schedule.h>
#include <calendar.h>

#include <boost/decimal.hpp>

#include <chrono>
#include <utility>
#include <stdexcept>

using namespace std;
using namespace std::chrono;
using namespace boost::decimal;
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

		constexpr auto rfd = rate_fixings_detail{ actual_360<decimal128_t>{} };

		// Compound from 2026-04-02 (business) to 2026-04-06 (business) across Good Friday
		constexpr auto rd = rate_detail{ 2026y / April / 2d, 2026y / April / 6d, rfd.day_count, 5u };
		const/*expr*/ auto cd = compounded_detail{fix.get_calendar()};

		const auto result = compounded(fix, rfd, cd, rd);

		// For this case there is a single step using the 2026-04-02 fixing over 4 days.
		// The computed compounded annualised rate should equal the underlying daily rate (1.80%)
		EXPECT_EQ(Percent{ "1.80" }, result.percent);
	}

	TEST(compounded, compounded2)
	{
		// Create a tiny SONIA-like IndexFixings around a single weekend
		// Period: 2026-04-02 .. 2026-04-06
		auto ts = IndexFixings::storage{ days_period{ 2026y / April / 2d, 2026y / April / 6d } };
		// Business days: 2026-04-02 (Thu), 2026-04-03 (Fri), 2026-04-06 (Mon)
		ts[2026y / April / 2d] = Value{ "100" };
		ts[2026y / April / 3d] = Value{ "100.005" };
		// weekend 4/4 and 4/5 left empty (non-business, no fixing)
		ts[2026y / April / 6d] = Value{ "100.010" };

		auto cal = calendar{ SaturdaySundayWeekend, schedule{ ts.get_period(), {} } };

		const auto fix = fixings{ std::move(ts), std::move(cal), 3u };

		constexpr auto dc = actual_360<decimal128_t>{};

		// compounded(const IndexFixings&, rate_detail) currently uses fix[ymd] (plain
		// lookup, no fallback) for both the start and end date - so requesting a start
		// or end date that falls on a non business day throws, it does NOT fall back
		// to the preceding business day's published index value.
		const auto detail1 = rate_detail{ 2026y / April / 4d, 2026y / April / 6d, dc, 5u }; // Saturday start
		EXPECT_THROW(static_cast<void>(compounded(fix, detail1)), runtime_error); // or should a fallback kick in?

		const auto detail2 = rate_detail{ 2026y / April / 2d, 2026y / April / 5d, dc, 5u }; // Sunday end
		EXPECT_THROW(static_cast<void>(compounded(fix, detail2)), runtime_error); // or should a fallback kick in?

		// should we also test a good run?
	}

}
