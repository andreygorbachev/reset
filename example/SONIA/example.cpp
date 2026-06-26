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

#include <scaled_value.h>
#include <fixings.h>
#include <index.h>

#include <actual_365_fixed.h>

#include <static_data.h>
#include <calendar.h>
#include <schedule.h>
#include <period.h>

#include <boost/decimal.hpp>

#include <chrono>
#include <iostream>
#include <iomanip>
#include <ios>
#include <algorithm>
#include <iterator>
#include <cassert>

using namespace std;
using namespace std::chrono;

using namespace boost::decimal;
using namespace boost::decimal::literals;

using namespace gregorian;
using namespace gregorian::util;
using namespace gregorian::static_data;

using namespace fin_calendar;

using namespace reset;



static auto parse_csv_fixings_SONIA() -> RateFixings
{
	// from https://www.bankofengland.co.uk/markets/sonia-benchmark
	return parse_csv_fixings<RateFixings>(
		"SONIA.csv",
		1997y / January / 1d,
		2025y / May / 12d,
		4u
	);
}

static auto parse_csv_fixings_SONIA_compounded_index() -> IndexFixings
{
	// from https://www.bankofengland.co.uk/markets/sonia-benchmark
	return parse_csv_fixings<IndexFixings>(
		"SONIA Compounded Index.csv",
		2018y / April / 23d,
		2025y / May / 13d,
		8u
	);
}



int main()
{
	const auto SONIA = parse_csv_fixings_SONIA();

	const auto rfd = rate_fixings_detail{
		.day_count = actual_365_fixed<decimal128_t>{}
	};

	const auto SONIA_compounded_index = parse_csv_fixings_SONIA_compounded_index();
	// I think BoE website does not fully describe the compounded index
	// more clarity would be welcome there on how rounding is done daily (*)

	constexpr auto id = index_detail{
		.initial_value = 100_DL,
		.initial_date = 2018y / April / 23d,
		.step_round = 18u,
		.final_round = 8u
	};

	const auto date = 2025y / May / 13d;

	const auto& indx = SONIA_compounded_index[date];
	assert(indx);

	// check the latest data available in this example
	cout
		<< fixed
		<< setprecision(SONIA_compounded_index.get_decimal_places())
		<< "For "
		<< date
		<< " SONIA Compounded Index is "
		<< indx->get_value()
		<< " and the same computed value is "
		<< index(SONIA, rfd, date, id).get_value()
		<< endl;

	// look for inconsistencies in the index data
	const auto period = SONIA_compounded_index.get_time_series().get_period();
	for (
		auto d = period.get_from();
		d <= period.get_until();
		d = sys_days{ d } + days{ 1 }
	)
	{
		const auto& fix = SONIA_compounded_index[d];
		if (fix)
		{
			const auto computed_fix = index(SONIA, rfd, d, id);
			if (*fix != computed_fix)
				cout
					<< "For "
					<< d
					<< " SONIA Compounded Index is "
					<< fix->get_value()
					<< " and the same computed value is "
					<< computed_fix.get_value()
					<< endl;
		}
	}

	return 0;
}
