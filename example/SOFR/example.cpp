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
#include <fixings.h>
#include <index.h>
#include <average.h>

#include <actual_360.h>

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



static auto parse_csv_fixings_SOFR() -> RateFixings
{
	// from https://www.newyorkfed.org/markets/reference-rates/sofr
	return parse_csv_fixings<RateFixings>(
		"SOFR.csv",
		1u, // skip the "SOFR," column after the date
		2018y / April / 2d,
		2026y / April / 9d,
		2
	);
}

static auto parse_csv_fixings_SOFR_compounded_index() -> IndexFixings
{
	// from https://www.newyorkfed.org/markets/reference-rates/sofr-averages-and-index
	return parse_csv_fixings<IndexFixings>(
		"SOFR Compounded Index.csv", // also includes averages, so maybe needs a better name
		15u,
		2020y / March / 2d,
		2026y / April / 10d,
		8
	);
}

static auto parse_csv_fixings_SOFR_30_day_average() -> RateFixings
{
	return parse_csv_fixings<RateFixings>(
		"SOFR Compounded Index.csv",
		12u,
		2020y / March / 2d,
		2026y / April / 10d,
		5
	);
}

static auto parse_csv_fixings_SOFR_90_day_average() -> RateFixings
{
	return parse_csv_fixings<RateFixings>(
		"SOFR Compounded Index.csv",
		13u,
		2020y / March / 2d,
		2026y / April / 10d,
		5
	);
}

static auto parse_csv_fixings_SOFR_180_day_average() -> RateFixings
{
	return parse_csv_fixings<RateFixings>(
		"SOFR Compounded Index.csv",
		14u,
		2020y / March / 2d,
		2026y / April / 10d,
		5
	);
}



int main()
{
	const auto SOFR = parse_csv_fixings_SOFR();

	auto rfd = rate_fixing_detail{};
	rfd.day_count = actual_360<Decimal>{};

	const auto SOFR_compounded_index = parse_csv_fixings_SOFR_compounded_index();

	const auto SOFR_30_day_average = parse_csv_fixings_SOFR_30_day_average();
	assert(SOFR_30_day_average.get_calendar() == SOFR_compounded_index.get_calendar());
	const auto SOFR_90_day_average = parse_csv_fixings_SOFR_90_day_average();
	assert(SOFR_90_day_average.get_calendar() == SOFR_compounded_index.get_calendar());
	const auto SOFR_180_day_average = parse_csv_fixings_SOFR_180_day_average();
	assert(SOFR_180_day_average.get_calendar() == SOFR_compounded_index.get_calendar());

	// from https://www.newyorkfed.org/markets/opolicy/operating_policy_200212
	auto id = index_detail{};
	id.initial_value = Decimal{ 1 };
	id.initial_date = 2018y / April / 2d;
	id.final_round = 8u;

	auto _30dd = average_detail{};
	_30dd.term = days{ 30 };
	_30dd.final_round = 5u + 2u; // as we deal with fractions, rather than rates

	auto _90dd = average_detail{};
	_90dd.term = days{ 90 };
	_90dd.final_round = 5u + 2u; // as we deal with fractions, rather than rates

	auto _180dd = average_detail{};
	_180dd.term = days{ 180 };
	_180dd.final_round = 5u + 2u; // as we deal with fractions, rather than rates

	//	const auto date = 2026y / April / 10d;
	const auto date = 2026y / April / 9d;

	// check the latest data available in this example
	cout
		<< fixed
		<< setprecision(8)
		<< "For "
		<< date
		<< " SOFR Compounded Index is "
		<< SOFR_compounded_index[date]->get_value()
		<< " and the same computed value is "
		<< index(SOFR, rfd, date, id).get_value()
		<< endl;

	cout
		<< fixed
		<< setprecision(5)
		<< "For "
		<< date
		<< " SOFR 30 Day Average is "
		<< SOFR_30_day_average[date]->get_value()
		<< " and the same computed value is "
		<< average(SOFR, rfd, date, _30dd).percent.get_value()
		<< endl;

	cout
		<< fixed
		<< setprecision(5)
		<< "For "
		<< date
		<< " SOFR 90 Day Average is "
		<< SOFR_90_day_average[date]->get_value()
		<< " and the same computed value is "
		<< average(SOFR, rfd, date, _90dd).percent.get_value()
		<< endl;

	cout
		<< fixed
		<< setprecision(5)
		<< "For "
		<< date
		<< " SOFR 180 Day Average is "
		<< SOFR_180_day_average[date]->get_value()
		<< " and the same computed value is "
		<< average(SOFR, rfd, date, _180dd).percent.get_value()
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
	const auto index_dates = SOFR_compounded_index_calendar.make_business_days_schedule(
		SOFR_compounded_index.get_time_series().get_period()
	);
	for (const auto& d : index_dates.get_dates())
	{
		if (d == *index_dates.get_dates().crbegin())
			break;
		// temporary only, unit we sort out start/end of RFR/RFR Index

		if (*SOFR_compounded_index[d] != index(SOFR, rfd, d, id))
			cout
				<< fixed
				<< setprecision(8)
				<< "For "
				<< d
				<< " SOFR Compounded Index is "
				<< SOFR_compounded_index[d]->get_value()
				<< " and the same computed value is "
				<< index(SOFR, rfd, d, id).get_value()
				<< endl;
	}

	const auto& SOFR_30_day_average_calendar = SOFR_30_day_average.get_calendar();
	const auto _30_day_dates = SOFR_30_day_average_calendar.make_business_days_schedule(
		SOFR_30_day_average.get_time_series().get_period()
	);
	for (const auto& d : _30_day_dates.get_dates())
	{
		if (d == *_30_day_dates.get_dates().crbegin())
			break;
		// temporary only

		if (*SOFR_30_day_average[d] != average(SOFR, rfd, d, _30dd).percent)
			cout
				<< fixed
				<< setprecision(5)
				<< "For "
				<< d
				<< " SOFR 30 Day Average is "
				<< SOFR_30_day_average[d]->get_value()
				<< " and the same computed value is "
				<< average(SOFR, rfd, d, _30dd).percent.get_value()
				<< endl;
	}

	const auto& SOFR_90_day_average_calendar = SOFR_90_day_average.get_calendar();
	const auto _90_day_dates = SOFR_90_day_average_calendar.make_business_days_schedule(
		SOFR_90_day_average.get_time_series().get_period()
	);
	for (const auto& d : _90_day_dates.get_dates())
	{
		if (d == *_90_day_dates.get_dates().crbegin())
			break;
		// temporary only

		if (*SOFR_90_day_average[d] != average(SOFR, rfd, d, _90dd).percent)
			cout
			<< fixed
			<< setprecision(5)
			<< "For "
			<< d
			<< " SOFR 90 Day Average is "
			<< SOFR_90_day_average[d]->get_value()
			<< " and the same computed value is "
			<< average(SOFR, rfd, d, _90dd).percent.get_value()
			<< endl;
	}

	const auto& SOFR_180_day_average_calendar = SOFR_180_day_average.get_calendar();
	const auto _180_day_dates = SOFR_180_day_average_calendar.make_business_days_schedule(
		SOFR_180_day_average.get_time_series().get_period()
	);
	for (const auto& d : _180_day_dates.get_dates())
	{
		if (d == *_180_day_dates.get_dates().crbegin())
			break;
		// temporary only

		if (*SOFR_180_day_average[d] != average(SOFR, rfd, d, _180dd).percent)
			cout
			<< fixed
			<< setprecision(5)
			<< "For "
			<< d
			<< " SOFR 180 Day Average is "
			<< SOFR_180_day_average[d]->get_value()
			<< " and the same computed value is "
			<< average(SOFR, rfd, d, _180dd).percent.get_value()
			<< endl;
	}

	return 0;
}

// what should we expect for 03 April 2026? there was no SOFR published on that day, but would compounding be done on that day?
// (what is the compounding calendar for the index and averages?)

// do we also need to cosider FedFunds?
