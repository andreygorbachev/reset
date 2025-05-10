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

#include <cmath>

#include <boost/multiprecision/cpp_dec_float.hpp>

#include <resets_math.h>

#include <gtest/gtest.h>

using namespace boost::multiprecision;

using namespace std;


namespace reset
{

	TEST(resets_math, round_dp1)
	{
		EXPECT_EQ(1.01, round_dp(1.011111, 2u));
	}

	TEST(resets_math, round_dp2)
	{
		EXPECT_EQ(cpp_dec_float_50{ "1.01" }, round_dp(cpp_dec_float_50{ "1.011111" }, 2u));
	}

	TEST(resets_math, trunc_dp1)
	{
		EXPECT_EQ(1.01, trunc_dp(1.011111, 2u));
	}

	TEST(resets_math, trunc_dp2)
	{
		EXPECT_EQ(cpp_dec_float_50{ "1.01" }, trunc_dp(cpp_dec_float_50{ "1.011111" }, 2u));
	}


	TEST(resets_math, from_percent1)
	{
		EXPECT_EQ(0.01, from_percent(1.0));
	}

	TEST(resets_math, from_percent2)
	{
		EXPECT_EQ(cpp_dec_float_50{ "0.01" }, from_percent(cpp_dec_float_50{ "1.0" }));
	}

	TEST(resets_math, to_percent1)
	{
		EXPECT_EQ(1.0, to_percent(0.01));
	}

	TEST(resets_math, to_percent2)
	{
		EXPECT_EQ(cpp_dec_float_50{ "1.0" }, to_percent(cpp_dec_float_50{ "0.01" }));
	}

}
