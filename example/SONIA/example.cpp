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

	auto rfd = rate_fixing_detail{};
	rfd.day_count = actual_365_fixed<Decimal>{};

	const auto SONIA_compounded_index = parse_csv_fixings_SONIA_compounded_index();
	// I think BoE website does not fully describe the compounded index
	// more clarity would be welcome there on how rounding is done daily (*)

	auto id = index_detail{};
	id.initial_value = Value{ "100" };
	id.initial_date = 2018y / April / 23d;
	id.step_round = 18u;
	id.final_round = 8u;

//	const auto date = 2025y / May / 13d;
	const auto date = 2025y / May / 12d;

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

	// see if we need a fallback
	const auto& London_calendar = locate_calendar("Europe/London", date);

	const auto dates =
//		London_calendar.make_business_days_schedule(SONIA.get_time_series().get_period());
		London_calendar.make_business_days_schedule(days_period{ 2018y / April / 23d, date });
	for (const auto& d : dates.get_dates())
		if (SONIA.fallback(d))
			cout
				<< "Fallback was needed on "
				<< d
				<< endl;

	// see if we have any unexpected fixings
	for (
		auto d = Epoch.get_from(); // should really check that it is not before SONIA started
		d <= date;
		d = sys_days{ d } + days{ 1 }
	)
		if(London_calendar.is_non_business_day(d) && SONIA[d])
			cout
				<< "For "
				<< d
				<< " we have a SONIA fixing, but no fixing was expected"
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
