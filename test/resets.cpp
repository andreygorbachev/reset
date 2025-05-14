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

#include <day_count.h>

#include <time_series.h>

#include <boost/multiprecision/cpp_dec_float.hpp>

#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <optional>


using namespace fin_calendar;

using namespace gregorian;

using namespace boost::multiprecision;

using namespace std;
using namespace std::chrono;


namespace reset
{

	TEST(resets, constructor)
	{
		auto ts = _time_series<optional<cpp_dec_float_50>>{ days_period{ 2023y / January / 1d, 2023y / June / 5d } };

		auto c = calendar{ SaturdaySundayWeekend, schedule{ ts.get_period(), {} } };

		const auto rs = resets{ move(ts), move(c), actual_365_fixed<cpp_dec_float_50>{} };

		const auto expected1 = _time_series<optional<cpp_dec_float_50>>{ days_period{ 2023y / January / 1d, 2023y / June / 5d } };
		const auto expected2 = calendar{ SaturdaySundayWeekend, schedule{ ts.get_period(), {} } };
		const auto expected3 = day_count<cpp_dec_float_50>{ actual_365_fixed<cpp_dec_float_50>{} };
		EXPECT_EQ(expected1, rs.get_time_series());
//		EXPECT_EQ(expected2, rs.get_publication_calendar());
//		EXPECT_EQ(expected3, rs.get_day_count());
	}

	TEST(resets, operator_square_brackets)
	{
		auto ts = _time_series<optional<cpp_dec_float_50>>{ days_period{ 2023y / January / 1d, 2023y / June / 5d } };
		ts[2023y / January / 3d] = cpp_dec_float_50{ "3.4269" };

		auto c = calendar{ SaturdaySundayWeekend, schedule{ ts.get_period(), {} } };

		const auto rs = resets{ move(ts), move(c), actual_365_fixed<cpp_dec_float_50>{} };

		EXPECT_EQ(cpp_dec_float_50{ "0.034269" }, rs[2023y / January / 3d]);

		EXPECT_THROW(rs[2023y / January / 1d], out_of_range);
	}

	TEST(resets, last_reset_year_month_day)
	{
		auto ts = _time_series<optional<cpp_dec_float_50>>{ days_period{ 2023y / January / 1d, 2023y / June / 5d } };
		ts[2023y / January / 3d] = cpp_dec_float_50{ "3.4269" };

		auto c = calendar{ SaturdaySundayWeekend, schedule{ ts.get_period(), {} } };

		const auto rs = resets{ move(ts), move(c), actual_365_fixed<cpp_dec_float_50>{} };

		EXPECT_EQ(2023y / January / 3d, rs.last_reset_year_month_day());
	}

}
