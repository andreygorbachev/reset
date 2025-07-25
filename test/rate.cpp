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
#include <cmath>

#include <static_data.h>

#include <day_count.h>

#include <rate.h>

#include <gtest/gtest.h>

using namespace std;
using namespace std::chrono;
using namespace boost::multiprecision;
using namespace fin_calendar;
using namespace gregorian;
using namespace gregorian::static_data;


namespace reset
{

	TEST(simple_annualized, interest)
	{
		const auto r = simple_annualized{ 6.0 };

		const auto i = r.interest(
			year_month_day{ 2025y / January / 1d },
			year_month_day{ 2025y / January / 31d },
			actual_365_fixed{}
		);

		const auto expected = 6.0 / 100.0 * 30.0 / 365.0;
		EXPECT_EQ(expected, i);
	}

	TEST(compound_annualized, interest)
	{
		const auto& c = locate_calendar("America/ANBIMA"s);

		const auto r = compound_annualized{ 6.0 };

		const auto i = r.interest(
			year_month_day{ 2025y / January / 1d },
			year_month_day{ 2025y / January / 31d },
			calculation_252{ c }
		);

		const auto expected = pow(1.0 + 6.0 / 100.0, 21.0 / 252.0) - 1.0; // we have 21 business days in that period
		EXPECT_EQ(expected, i);
	}

	TEST(rate, rate)
	{
		const auto& c = locate_calendar("America/ANBIMA"s);

		const auto r1 = rate<cpp_dec_float_50>{ simple_annualized<cpp_dec_float_50>{ 5 } };
		const auto dc1 = actual_365_fixed<cpp_dec_float_50>{};
		const auto i1 = interest(
			year_month_day{ 2023y / January / 1d },
			year_month_day{ 2023y / January / 2d },
			day_count<cpp_dec_float_50>{ dc1 },
			r1
		);

		const auto r2 = rate<cpp_dec_float_50>{ compound_annualized<cpp_dec_float_50>{ 5 } };
		const auto dc2 = calculation_252<cpp_dec_float_50>{ c };
		const auto i2 = interest(
			year_month_day{ 2023y / January / 1d },
			year_month_day{ 2023y / January / 2d },
			day_count<cpp_dec_float_50>{ dc2 },
			r2
		);
	}

}
