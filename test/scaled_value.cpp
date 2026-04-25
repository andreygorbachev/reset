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

#include <decimal.h>
#include <scaled_value.h>

#include <gtest/gtest.h>

using namespace std;


namespace reset
{

	TEST(scaled_value, constructor1)
	{
		const auto sv = Percent{ "3" };
		// also test "wrong" strings I guess (or do we just rely on cpp_dec_float_50 to do so)
		EXPECT_EQ(Percent{ "3" }, sv);
	}

	TEST(scaled_value, constructor2)
	{
		const auto sv = Percent{ Decimal{ "0.03" } }; // 3%
		EXPECT_EQ(Percent{ "3" }, sv);
	}

	TEST(scaled_value, operator_Decimal)
	{
		const auto sv = Percent{ "3" };
		EXPECT_EQ(Decimal{ "0.03" }, static_cast<Decimal>(sv));
	}

	TEST(scaled_value, get_value)
	{
		const auto sv = Percent{ "3" };
		EXPECT_EQ(Decimal{ "3" }, sv.get_value());
	}

	TEST(scaled_value, Percent)
	{
		const auto sv = Percent{ "3" };
		EXPECT_EQ(Decimal{ "0.03" }, static_cast<Decimal>(sv));
	}

	TEST(scaled_value, BasisPoints)
	{
		const auto sv = BasisPoints{ "3" };
		EXPECT_EQ(Decimal{ "0.0003" }, static_cast<Decimal>(sv));
	}

	TEST(scaled_value, Value)
	{
		const auto sv = Value{ "3" };
		EXPECT_EQ(Decimal{ "3" }, static_cast<Decimal>(sv));
	}

	// do we need to test default functions?

}
