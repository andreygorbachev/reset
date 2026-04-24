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

using namespace std;
using namespace std::chrono;

using namespace boost::multiprecision;

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
		4
	);
}

static auto parse_csv_fixings_SONIA_compounded_index() -> IndexFixings
{
	// from https://www.bankofengland.co.uk/markets/sonia-benchmark
	return parse_csv_fixings<IndexFixings>(
		"SONIA Compounded Index.csv",
		2018y / April / 23d,
		2025y / May / 13d,
		8
	);
}



int main()
{
	const auto SONIA = parse_csv_fixings_SONIA();

	auto rfd = rate_fixing_detail{};
	rfd.day_count = actual_365_fixed<cpp_dec_float_50>{};

	const auto SONIA_compounded_index = parse_csv_fixings_SONIA_compounded_index();
	// I think BoE website does not fully describe the compounded index
	// more clarity would be welcome there on how rounding is done daily (*)

	auto id = index_detail{};
	id.initial_value = cpp_dec_float_50{ 100 };
	id.initial_date = 2018y / April / 23d;
	id.step_round = 18u;
	id.final_round = 8u;

//	const auto date = 2025y / May / 13d;
	const auto date = 2025y / May / 12d;

	// check the latest data available in this example
	cout
		<< fixed
		<< setprecision(SONIA_compounded_index.get_decimal_places())
		<< "For "
		<< date
		<< " SONIA Compounded Index is "
		<< SONIA_compounded_index[date]->get_value()
		<< " and the same computed value is "
		<< index(SONIA, rfd, date, id).get_value()
		<< endl;

	const auto& London_calendar = locate_calendar("Europe/London", date);

	// check the SONIA dates
	const auto& SONIA_calendar = SONIA.get_calendar();
	const auto common_period_1 = SONIA_calendar.get_schedule().get_period() & London_calendar.get_schedule().get_period();
	if (calendar{ SONIA_calendar.get_weekend(), schedule{ common_period_1, SONIA_calendar.get_schedule().get_dates() } } ==
		calendar{ London_calendar.get_weekend(), schedule{ common_period_1, London_calendar.get_schedule().get_dates() } }
	)
		cout << "SONIA calendar and London calendar match" << endl;
	else
	{
		cout << "SONIA calendar and London calendar do not match" << endl;

		auto diffs = schedule::dates{};
		ranges::set_symmetric_difference(
			SONIA_calendar.make_business_days_schedule(common_period_1).get_dates(),
			London_calendar.make_business_days_schedule(common_period_1).get_dates(),
			inserter(diffs, diffs.begin())
		);
		cout << "The following dates are in one calendar but not in the other:" << endl;
		for(const auto& d : diffs)
			cout << d << endl;
	}

	// check the SONIA Compounded Index dates
	const auto& SONIA_compounded_index_calendar = SONIA_compounded_index.get_calendar();
	const auto common_period_2 = SONIA_compounded_index_calendar.get_schedule().get_period() & London_calendar.get_schedule().get_period();
	if (calendar{ SONIA_compounded_index_calendar.get_weekend(), schedule{ common_period_2, SONIA_compounded_index_calendar.get_schedule().get_dates() } } ==
		calendar{ London_calendar.get_weekend(), schedule{ common_period_2, London_calendar.get_schedule().get_dates() } }
	)
		cout << "SONIA Compounded Index calendar and London calendar match" << endl;
	else
	{
		cout << "SONIA Compounded Index calendar and London calendar do not match" << endl;

		auto diffs = schedule::dates{};
		ranges::set_symmetric_difference(
			SONIA_compounded_index_calendar.make_business_days_schedule(common_period_2).get_dates(),
			London_calendar.make_business_days_schedule(common_period_2).get_dates(),
			inserter(diffs, diffs.begin())
		);
		cout << "The following dates are in one calendar but not in the other:" << endl;
		for (const auto& d : diffs)
			cout << d << endl;
	}

	// look for inconsistencies in the data
	const auto dates = SONIA_compounded_index_calendar.make_business_days_schedule(
		SONIA_compounded_index.get_time_series().get_period()
	);
	for (const auto& d : dates.get_dates())
	{
		if (d == *dates.get_dates().crbegin())
			break;
		// temporary only, until we sort out start/end of RFR/RFR Index

		if (*SONIA_compounded_index[d] != index(SONIA, rfd, d, id))
			cout
				<< "For "
				<< d
				<< " SONIA Compounded Index is "
				<< SONIA_compounded_index[d]->get_value()
				<< " and the same computed value is "
				<< index(SONIA, rfd, d, id).get_value()
				<< endl;
	}

	return 0;
}
