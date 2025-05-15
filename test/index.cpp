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

#include <stdexcept>

#include <boost/multiprecision/cpp_dec_float.hpp>

#include <index.h>
#include <resets.h>

#include <gtest/gtest.h>

#include "setup.h"

using namespace std;
using namespace std::chrono;
using namespace boost::multiprecision;


namespace reset
{

	TEST(index, index1)
	{
		// from "Statement Regarding Publication of SOFR Averages and a SOFR Index"

		const auto resets = make_SOFR_resets();

		auto detail = index_detail{};
		detail.initial_value = cpp_dec_float_50{ "1" };
		detail.initial_date = 2018y / April / 2d;
		detail.final_rounding = 8u;

//		EXPECT_EQ(resets::observation{ "1.00000000" }, index(resets, 2018y / April / 2d, detail));
		EXPECT_EQ(resets::observation{ "1.00005000" }, index(resets, 2018y / April / 3d, detail));
		EXPECT_EQ(resets::observation{ "1.00010084" }, index(resets, 2018y / April / 4d, detail));
		EXPECT_EQ(resets::observation{ "1.00014917" }, index(resets, 2018y / April / 5d, detail));
		EXPECT_EQ(resets::observation{ "1.00019779" }, index(resets, 2018y / April / 6d, detail));
//		EXPECT_EQ(resets::observation{ "1.00034365" }, index(resets, 2018y / April / 9d, detail));
	}

	TEST(index, index2)
	{
		// test the step rounding - maybe SONIA?
	}

	TEST(index, index3)
	{
		// test the factor rounding
	}

	TEST(index, index4)
	{
		const auto resets = make_SOFR_resets();

		// don't allow for an index to be generated before it actually exists
//		EXPECT_THROW(index(resets, 2018y / March / 29d), out_of_range); // the last business day before the SOFR started

		// don't allow for an index to be generated on a non-business day
//		EXPECT_THROW(index(resets, 2018y / April / 7d), out_of_range);
	}

}
