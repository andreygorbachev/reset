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
#include <average.h>

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
		"SOFR Compounded Index.csv", // also includes averages, so maybe needs a better name
		15u,
		2020y / March / 2d,
		2026y / April / 10d
	);
}

static auto parse_csv_resets_SOFR_30_day_average() -> resets
{
	return parse_csv_resets(
		"SOFR Compounded Index.csv",
		12u,
		2020y / March / 2d,
		2026y / April / 10d
	);
}

static auto parse_csv_resets_SOFR_90_day_average() -> resets
{
	return parse_csv_resets(
		"SOFR Compounded Index.csv",
		13u,
		2020y / March / 2d,
		2026y / April / 10d
	);
}

static auto parse_csv_resets_SOFR_180_day_average() -> resets
{
	return parse_csv_resets(
		"SOFR Compounded Index.csv",
		14u,
		2020y / March / 2d,
		2026y / April / 10d
	);
}



int main()
{
	const auto SOFR = parse_csv_resets_SOFR();

	const auto SOFR_compounded_index = parse_csv_resets_SOFR_compounded_index();

	const auto SOFR_30_day_average = parse_csv_resets_SOFR_30_day_average();
	const auto SOFR_90_day_average = parse_csv_resets_SOFR_90_day_average();
	const auto SOFR_180_day_average = parse_csv_resets_SOFR_180_day_average();

	// from https://www.newyorkfed.org/markets/opolicy/operating_policy_200212
	auto id = index_detail{};
	id.initial_value = cpp_dec_float_50{ 1 };
	id.initial_date = 2018y / April / 2d;
	id.final_round = 8u;

	auto _30dd = average_detail{};
	_30dd.term = days{ 30 };
	_30dd.final_round = 5u + 2u; // as we deal with fractions, rather than rates

	auto _90dd = average_detail{};
	_90dd.term = days{ 90 };
	_90dd.final_round = 5u + 2u; // as we deal with fractions, rather than rates

	//	const auto date = 2026y / April / 10d;
	const auto date = 2026y / April / 9d;

	// check the latest data available in this example
	cout
		<< fixed
		<< setprecision(8)
		<< "For "
		<< date
		<< " SOFR Compounded Index is "
		<< SOFR_compounded_index[date] * 100 // need a different accessor? (or handle 100 in some other way)
		<< " and the same computed value is "
		<< index(SOFR, date, id)
		<< endl;

	cout
		<< fixed
		<< setprecision(5)
		<< "For "
		<< date
		<< " SOFR 30 Day Average is "
		<< SOFR_30_day_average[date] * 100 // need a different accessor? (or handle 100 in some other way)
		<< " and the same computed value is "
		<< average(SOFR, date, _30dd) * 100
		<< endl;

	cout
		<< fixed
		<< setprecision(5)
		<< "For "
		<< date
		<< " SOFR 90 Day Average is "
		<< SOFR_90_day_average[date] * 100 // need a different accessor? (or handle 100 in some other way)
		<< " and the same computed value is "
		<< average(SOFR, date, _90dd) * 100
		<< endl;

	const auto& SIFMA_calendar = locate_calendar("America/SIFMA", date);

	// check the SOFR dates
	const auto& SOFR_calendar = SOFR.get_calendar();
	const auto common_period_1 = SOFR_calendar.get_schedule().get_period() & SIFMA_calendar.get_schedule().get_period();
	if (calendar{ SOFR_calendar.get_weekend(), schedule{ common_period_1, SOFR_calendar.get_schedule().get_dates() } } ==
		calendar{ SIFMA_calendar.get_weekend(), schedule{ common_period_1, SIFMA_calendar.get_schedule().get_dates() } }
	)
		cout << "SOFR calendar and USA calendar match" << endl;
	else
	{
		cout << "SOFR calendar and USA calendar do not match" << endl;

		auto diffs = schedule::dates{};
		ranges::set_symmetric_difference(
			SOFR_calendar.make_business_days_schedule(common_period_1).get_dates(),
			SIFMA_calendar.make_business_days_schedule(common_period_1).get_dates(),
			inserter(diffs, diffs.begin())
		);
		cout << "The following dates are in one calendar but not in the other:" << endl;
		for(const auto& d : diffs)
			cout << d << endl;
	}

	// check the SOFR Compounded Index dates
	const auto& SOFR_compounded_index_calendar = SOFR_compounded_index.get_calendar();
	const auto common_period_2 = SOFR_compounded_index_calendar.get_schedule().get_period() & SIFMA_calendar.get_schedule().get_period();
	if (calendar{ SOFR_compounded_index_calendar.get_weekend(), schedule{ common_period_2, SOFR_compounded_index_calendar.get_schedule().get_dates() } } ==
		calendar{ SIFMA_calendar.get_weekend(), schedule{ common_period_2, SIFMA_calendar.get_schedule().get_dates() } }
	)
		cout << "SOFR Compounded Index calendar and USA calendar match" << endl;
	else
	{
		cout << "SOFR Compounded Index calendar and USA calendar do not match" << endl;

		auto diffs = schedule::dates{};
		ranges::set_symmetric_difference(
			SOFR_compounded_index_calendar.make_business_days_schedule(common_period_2).get_dates(),
			SIFMA_calendar.make_business_days_schedule(common_period_2).get_dates(),
			inserter(diffs, diffs.begin())
		);
		cout << "The following dates are in one calendar but not in the other:" << endl;
		for (const auto& d : diffs)
			cout << d << endl;
	}

	// look for inconsistencies in the data
	const auto dates = SOFR_compounded_index_calendar.make_business_days_schedule(
		SOFR_compounded_index.get_time_series().get_period()
	);
	for (const auto& d : dates.get_dates())
	{
		if (d == *dates.get_dates().crbegin())
			break;
		// temporary only, unit we sort out start/end of RFR/RFR Index

		if (SOFR_compounded_index[d] * 100 != index(SOFR, d, id))
			cout
				<< fixed
				<< setprecision(8)
				<< "For "
				<< d
				<< " SOFR Compounded Index is "
				<< SOFR_compounded_index[d] * 100
				<< " and the same computed value is "
				<< index(SOFR, d, id)
				<< endl;
	}

	return 0;
}
