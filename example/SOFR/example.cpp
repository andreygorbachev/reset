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

#include <resets.h>
#include <index.h>

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
using namespace std::ranges;

using namespace boost::multiprecision;

using namespace gregorian;
using namespace gregorian::util;
using namespace gregorian::static_data;

using namespace reset;



static auto parse_csv_resets_SOFR() -> resets
{
	// from https://www.newyorkfed.org/markets/reference-rates/sofr
	return parse_csv_resets(
		"SOFR.csv",
		1u, // skip the "SOFR," column after the date
		2018y / April / 2d,
		2026y / April / 9d
	);
}

static auto parse_csv_resets_SOFR_compounded_index() -> resets
{
	// from https://www.newyorkfed.org/markets/reference-rates/sofr-averages-and-index
	return parse_csv_resets(
		"SOFR Compounded Index.csv", // also inludes averages, so maybe needs a better name
		15u,
		2018y / April / 2d,
		2026y / April / 10d
	);
}



int main()
{
	const auto SOFR = parse_csv_resets_SOFR();

	const auto SOFR_compounded_index = parse_csv_resets_SOFR_compounded_index();
/*
	auto detail = index_detail{};
	detail.initial_value = cpp_dec_float_50{ 100 };
	detail.initial_date = 2018y / April / 23d;
	detail.step_round = 18u;
	detail.final_round = 8u;

//	const auto date = 2025y / May / 13d;
	const auto date = 2025y / May / 12d;

	// check the latest data available in this example
	cout
		<< fixed
		<< setprecision(8)
		<< "For "
		<< date
		<< " SOFR Compounded Index is "
		<< SOFR_compounded_index[date] * 100 // need a different accessor? (or handle 100 in some other way)
		<< " and the same computed value is "
		<< index(SOFR, date, detail)
		<< endl;

	const auto& London_calendar = locate_calendar("Europe/London", date);

	// check the SOFR dates
	const auto& SOFR_calendar = SOFR.get_calendar();
	const auto common_period_1 = SOFR_calendar.get_schedule().get_period() & London_calendar.get_schedule().get_period();
	if (calendar{ SOFR_calendar.get_weekend(), schedule{ common_period_1, SOFR_calendar.get_schedule().get_dates() } } ==
		calendar{ London_calendar.get_weekend(), schedule{ common_period_1, London_calendar.get_schedule().get_dates() } }
	)
		cout << "SOFR calendar and London calendar match" << endl;
	else
	{
		cout << "SOFR calendar and London calendar do not match" << endl;

		auto diffs = schedule::dates{};
		ranges::set_symmetric_difference(
			SOFR_calendar.make_business_days_schedule(common_period_1).get_dates(),
			London_calendar.make_business_days_schedule(common_period_1).get_dates(),
			inserter(diffs, diffs.begin())
		);
		cout << "The following dates are in one calendar but not in the other:" << endl;
		for(const auto& d : diffs)
			cout << d << endl;
	}

	// check the SOFR Compounded Index dates
	const auto& SOFR_compounded_index_calendar = SOFR_compounded_index.get_calendar();
	const auto common_period_2 = SOFR_compounded_index_calendar.get_schedule().get_period() & London_calendar.get_schedule().get_period();
	if (calendar{ SOFR_compounded_index_calendar.get_weekend(), schedule{ common_period_2, SOFR_compounded_index_calendar.get_schedule().get_dates() } } ==
		calendar{ London_calendar.get_weekend(), schedule{ common_period_2, London_calendar.get_schedule().get_dates() } }
	)
		cout << "SOFR Compounded Index calendar and London calendar match" << endl;
	else
	{
		cout << "SOFR Compounded Index calendar and London calendar do not match" << endl;

		auto diffs = schedule::dates{};
		ranges::set_symmetric_difference(
			SOFR_compounded_index_calendar.make_business_days_schedule(common_period_2).get_dates(),
			London_calendar.make_business_days_schedule(common_period_2).get_dates(),
			inserter(diffs, diffs.begin())
		);
		cout << "The following dates are in one calendar but not in the other:" << endl;
		for (const auto& d : diffs)
			cout << d << endl;
	}

	// look for inconsistencies in the data
	const auto dates = SOFR_compounded_index_calendar.make_business_days_schedule( days_period{ detail.initial_date, date } );
	for (const auto& d : dates.get_dates())
		if (SOFR_compounded_index[d] * 100 != index(SOFR, d, detail))
			cout
				<< "For "
				<< d
				<< " SOFR Compounded Index is "
				<< SOFR_compounded_index[d] * 100
				<< " and the same computed value is "
				<< index(SOFR, d, detail)
				<< endl;
*/
	return 0;
}
