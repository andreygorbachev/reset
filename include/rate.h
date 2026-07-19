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

#pragma once

#include "scaled_value.h"

#include <day_count.h>

#include <boost/decimal.hpp>

#include <chrono>


namespace reset
{

	struct rate_detail
	{

		std::chrono::year_month_day start; // usually included
		std::chrono::year_month_day end; // usually excluded

		fin_calendar::day_count<boost::decimal::decimal128_t> day_count;

		unsigned int round; // should this be optional? does it need to be unsigned?

	};

	struct rate
	{

		Percent percent;

		rate_detail detail;

	};
	// here we bring together the rate and its details, but we do not do that for fixings and their details, which approach is correct?

}
