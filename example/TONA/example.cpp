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

#include "parser.h"

#include <decimal.h>
#include <scaled_value.h>
#include <fixings.h>
#include <index.h>
#include <average.h>

#include <actual_365_fixed.h>

#include <static_data.h>
#include <calendar.h>
#include <schedule.h>
#include <period.h>

#include <chrono>
#include <iostream>
#include <iomanip>
#include <ios>
#include <algorithm>
#include <iterator>
#include <cassert>

using namespace std;
using namespace std::chrono;

using namespace gregorian;
using namespace gregorian::util;
using namespace gregorian::static_data;

using namespace fin_calendar;

using namespace reset;



static auto parse_csv_fixings_TONA() -> RateFixings
{
	// from https://www.stat-search.boj.or.jp/index_en.html
	return parse_csv_fixings<RateFixings>(
		"FM01.csv",
		1998y / January / 5d,
		2026y / May / 18d,
		3u
	);
}



int main()
{
	const auto TONA = parse_csv_fixings_TONA();

	auto rfd = rate_fixing_detail{};
	rfd.day_count = actual_365_fixed<Decimal>{};

	auto id = index_detail{};
	id.initial_value = Value{ "100" };
	id.initial_date = 2017y / March / 8d;
	id.step_round = 5u;

	// I think that (compounded) averages are calculated via the index, rather than "long formula"

	return 0;
}
