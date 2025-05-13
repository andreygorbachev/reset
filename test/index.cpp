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

#include <utility>

#include <boost/multiprecision/cpp_dec_float.hpp>

#include <period.h>

#include <actual_360.h>

#include <index.h>
#include <resets.h>

#include <gtest/gtest.h>

using namespace boost::multiprecision;

using namespace std;
using namespace std::chrono;
using namespace fin_calendar;
using namespace gregorian;


namespace reset
{

	TEST(index, index1)
	{
		// from "Statement Regarding Publication of SOFR Averages and a SOFR Index"

		// factor out into a setup function? (so here the rs can be const)
		/*const*/ auto rs = resets::storage{ days_period{2018y / April / 3d, 2018y / April / 9d} }; // should we write code to use initialiser for _time_series?
		rs[2018y / April / 3d] = resets::observation{ "1.80" };
		rs[2018y / April / 4d] = resets::observation{ "1.83" };
		rs[2018y / April / 5d] = resets::observation{ "1.74" };
		rs[2018y / April / 6d] = resets::observation{ "1.75" };
		rs[2018y / April / 9d] = resets::observation{ "1.75" };

		const auto resets = reset::resets{ move(rs), actual_360<resets::observation>{} };

		EXPECT_EQ(resets::observation{"1.00000000"}, index(resets, 2018y / April / 2d));
	}

}
