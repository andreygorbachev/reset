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

#include <fixings.h>

#include <time_series.h>

#include <gtest/gtest.h>

#include <chrono>
#include <optional>


using namespace fin_calendar;

using namespace gregorian;
using namespace gregorian::util;

using namespace std;
using namespace std::chrono;


namespace reset
{

	TEST(fixings, constructor)
	{
		auto ts = time_series<optional<Percent>>{ days_period{ 2023y / January / 1d, 2023y / June / 5d } };

		auto c = calendar{ SaturdaySundayWeekend, schedule{ ts.get_period(), {} } };

		const auto expected1 = time_series<optional<Percent>>{ days_period{ 2023y / January / 1d, 2023y / June / 5d } };
		const auto expected2 = calendar{ SaturdaySundayWeekend, schedule{ ts.get_period(), {} } };

		const auto fix = fixings{ move(ts), move(c), 4 };
//		EXPECT_EQ(expected1, fix.get_time_series());
//		EXPECT_EQ(expected2, fix.get_publication_calendar());
	}

	TEST(fixings, operator_square_brackets)
	{
		auto ts = time_series<optional<Percent>>{ days_period{ 2023y / January / 1d, 2023y / June / 5d } };
		ts[2023y / January / 3d] = Percent{ "3.4269" };

		auto c = calendar{ SaturdaySundayWeekend, schedule{ ts.get_period(), {} } };

		const auto fix = fixings{ move(ts), move(c), 4 };

		EXPECT_EQ(Percent{ "3.4269" }, *fix[2023y / January / 3d]);

		EXPECT_THROW(fix[2022y / December / 31d], out_of_range);
		EXPECT_THROW(fix[2023y / June / 6d], out_of_range);
	}

	TEST(fixings, current_observation)
	{
		auto ts = time_series<optional<Percent>>{ days_period{ 2023y / January / 1d, 2023y / June / 5d } };
		ts[2023y / January / 3d] = Percent{ "3.4269" };

		auto c = calendar{ SaturdaySundayWeekend, schedule{ ts.get_period(), {} } };

		const auto fix = fixings{ move(ts), move(c), 4 };

		EXPECT_EQ(Percent{ "3.4269" }, fix.current_observation(2023y / January / 3d));

		// test actual move when we ask for a non-business day (do we need to distinguish between non-business day and missing reset?)
		// test move before the start of fixings (is that possible?)
	}

	TEST(fixings, last_reset_year_month_day)
	{
		auto ts = time_series<optional<Percent>>{ days_period{ 2023y / January / 1d, 2023y / June / 5d } };
		ts[2023y / January / 3d] = Percent{ "3.4269" };

		auto c = calendar{ SaturdaySundayWeekend, schedule{ ts.get_period(), {} } };

		const auto fix = fixings{ move(ts), move(c), 4 };

		EXPECT_EQ(2023y / January / 3d, fix.last_reset_year_month_day());
	}

}
