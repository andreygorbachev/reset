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

#include <boost/multiprecision/cpp_dec_float.hpp>

#include <chrono>

#include <day_count.h>

#include <rate.h>

#include <gtest/gtest.h>

using namespace std;
using namespace std::chrono;
using namespace boost::multiprecision;
using namespace fin_calendar;


namespace reset
{

	TEST(simple, simple)
	{
		const auto r = simple{ 6.0 };

		const auto i = r.interest(
			year_month_day{ 2025y / January / 1d },
			year_month_day{ 2025y / January / 31d },
			actual_365_fixed{}
		);

		const auto expected = 6.0 / 100.0 * 30.0 / 365.0;
		EXPECT_EQ(expected, i);
	}

	TEST(compound, compound)
	{
		const auto r = compound{ 5.0 };

		const auto i = r.interest(
			year_month_day{ 2023y / January / 1d },
			year_month_day{ 2023y / January / 2d },
			actual_365_fixed{}
		);
	}

	TEST(rate, rate)
	{
		const auto r1 = rate<>{ simple{ 5.0 } };
		const auto dc1 = actual_365_fixed<double>{};
		const auto i1 = interest(
			year_month_day{ 2023y / January / 1d },
			year_month_day{ 2023y / January / 2d },
			day_count<>{ dc1 },
			r1
		);

		const auto r2 = rate<>{ compound{ 5.0 } };
		const auto dc2 = actual_365_fixed<double>{};
		const auto i2 = interest(
			year_month_day{ 2023y / January / 1d },
			year_month_day{ 2023y / January / 2d },
			day_count<>{ dc2 },
			r2
		);
	}

}
