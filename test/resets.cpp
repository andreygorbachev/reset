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

#include <resets.h>

#include <day_counts.h>

#include <time_series.h>

#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <optional>


using namespace coupon_schedule;

using namespace gregorian;

using namespace std;
using namespace std::chrono;


namespace risk_free_rate
{

	TEST(resets, constructor)
	{
		auto ts = _time_series<optional<double>>{ { 2023y / January / 1d, 2023y / June / 5d } };

		const auto rs = resets{ move(ts), &Actual365Fixed };

		const auto expected1 = _time_series<optional<double>>{ { 2023y / January / 1d, 2023y / June / 5d } };
		const auto expected2 = &Actual365Fixed;
		EXPECT_EQ(expected1, rs.get_time_series());
		EXPECT_EQ(expected2, rs.get_day_count());
	}

	TEST(resets, operator_square_brackets)
	{
		auto ts = _time_series<optional<double>>{ { 2023y / January / 1d, 2023y / June / 5d } };
		ts[2023y / January / 3d] = 3.4269;

		const auto rs = resets{ move(ts), &Actual365Fixed };

		EXPECT_EQ(0.034269, rs[2023y / January / 3d]);

		EXPECT_THROW(rs[2023y / January / 1d], out_of_range);
	}

	TEST(resets, last_reset_year_month_day)
	{
		auto ts = _time_series<optional<double>>{ { 2023y / January / 1d, 2023y / June / 5d } };
		ts[2023y / January / 3d] = 3.4269;

		const auto rs = resets{ move(ts), &Actual365Fixed };

		EXPECT_EQ(2023y / January / 3d, rs.last_reset_year_month_day());
	}


	TEST(resets, from_percent)
	{
		EXPECT_EQ(0.01, from_percent(1.0));
	}

	TEST(resets, to_percent)
	{
		EXPECT_EQ(1.0, to_percent(0.01));
	}

}
