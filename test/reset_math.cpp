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

#include <boost/decimal.hpp>

#include <reset_math.h>

#include <gtest/gtest.h>

using namespace std;
using namespace boost::decimal;
using namespace boost::decimal::literals;


namespace reset
{

	TEST(resets_math, round_dp1)
	{
		EXPECT_EQ(1.01, round_dp(1.011111, 2u));
	}

	TEST(resets_math, round_dp2)
	{
		// from
		// "Compounded €STR average rates and index"
		EXPECT_EQ(-1.00001_dl, round_dp(-1.000005_dl, 5u));
		EXPECT_EQ(1.00001_dl, round_dp(1.000005_dl, 5u));

		// to check that we do Rounding Half Up 
		EXPECT_EQ(3_dl, round_dp(2.5_dl, 0u));
//		EXPECT_EQ(2_dl, round_dp(2.5_dl, 0u)); // we do not do Banker's Rounding (Half-to-Even)
		EXPECT_EQ(4_dl, round_dp(3.5_dl, 0u));
	}

	TEST(resets_math, trunc_dp1)
	{
		EXPECT_EQ(1.01, trunc_dp(1.011111, 2u));
	}

	TEST(resets_math, trunc_dp2)
	{
		EXPECT_EQ(1.01_dl, trunc_dp(1.011111_dl, 2u));
	}

}
