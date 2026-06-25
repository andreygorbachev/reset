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

#include <parser.h>

#include <decimal.h>
#include <scaled_value.h>
#include <fixings.h>
#include <index.h>
#include <average.h>
#include <rate.h>
#include <compounded.h>

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
#include <optional>

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

	const auto d = parser_detail{
		.header_lines = 1u,
		.from = 2018y / April / 2d,
		.until = 2026y / April / 9d,
		.date_format = "%m/%d/%Y",
		.separator = ',',
		.padder = nullopt,
		.not_available = nullopt,
		.skip_columns = 1u
	};

	return parse_csv_fixings<RateFixings>(
		"SOFR.csv",
		d,
		"America/SOFR",
		2u
	);
}

static auto parse_csv_fixings_SOFR_compounded_index() -> IndexFixings
{
	// from https://www.newyorkfed.org/markets/reference-rates/sofr-averages-and-index

	const auto d = parser_detail{
		.header_lines = 1u,
		.from = 2020y / March / 2d, // get even earlier data?
		.until = 2026y / April / 10d,
		.date_format = "%m/%d/%Y",
		.separator = ',',
		.padder = nullopt,
		.not_available = nullopt,
		.skip_columns = 15u
	};

	return parse_csv_fixings<IndexFixings>(
		"SOFR Compounded Index.csv", // also includes averages, so maybe needs a better name
		d,
		"America/SOFR",
		8u
	);
}

static auto parse_csv_fixings_SOFR_30_day_average() -> RateFixings
{
	const auto d = parser_detail{
		.header_lines = 1u,
		.from = 2020y / March / 2d,
		.until = 2026y / April / 10d,
		.date_format = "%m/%d/%Y",
		.separator = ',',
		.padder = nullopt,
		.not_available = nullopt,
		.skip_columns = 12u
	};

	return parse_csv_fixings<RateFixings>(
		"SOFR Compounded Index.csv",
		d,
		"America/SOFR",
		5u
	);
}

static auto parse_csv_fixings_SOFR_90_day_average() -> RateFixings
{
	const auto d = parser_detail{
		.header_lines = 1u,
		.from = 2020y / March / 2d,
		.until = 2026y / April / 10d,
		.date_format = "%m/%d/%Y",
		.separator = ',',
		.padder = nullopt,
		.not_available = nullopt,
		.skip_columns = 13u
	};

	return parse_csv_fixings<RateFixings>(
		"SOFR Compounded Index.csv",
		d,
		"America/SOFR",
		5u
	);
}

static auto parse_csv_fixings_SOFR_180_day_average() -> RateFixings
{
	const auto d = parser_detail{
		.header_lines = 1u,
		.from = 2020y / March / 2d,
		.until = 2026y / April / 10d,
		.date_format = "%m/%d/%Y",
		.separator = ',',
		.padder = nullopt,
		.not_available = nullopt,
		.skip_columns = 14u
	};

	return parse_csv_fixings<RateFixings>(
		"SOFR Compounded Index.csv",
		d,
		"America/SOFR",
		5u
	);
}



int main()
{
	const auto SOFR = parse_csv_fixings_SOFR();

	const auto rfd = rate_fixings_detail{
		.day_count = actual_360<Decimal>{}
	};

	const auto SOFR_compounded_index = parse_csv_fixings_SOFR_compounded_index();

	const auto SOFR_30_day_average = parse_csv_fixings_SOFR_30_day_average();
	assert(SOFR_30_day_average.get_calendar() == SOFR_compounded_index.get_calendar());
	const auto SOFR_90_day_average = parse_csv_fixings_SOFR_90_day_average();
	assert(SOFR_90_day_average.get_calendar() == SOFR_compounded_index.get_calendar());
	const auto SOFR_180_day_average = parse_csv_fixings_SOFR_180_day_average();
	assert(SOFR_180_day_average.get_calendar() == SOFR_compounded_index.get_calendar());

	// from https://www.newyorkfed.org/markets/opolicy/operating_policy_200212
	const auto id = index_detail{
		.initial_value = Value{ "1" },
		.initial_date = 2018y / April / 2d,
		.final_round = 8u
	};

	const auto _30dd = average_detail{
		.term = days{ 30 },
		.final_round = 5u + 2u // as we deal with fractions, rather than rates
	};

	const auto _90dd = average_detail{
		.term = days{ 90 },
		.final_round = 5u + 2u // as we deal with fractions, rather than rates
	};

	const auto _180dd = average_detail{
		.term = days{ 180 },
		.final_round = 5u + 2u // as we deal with fractions, rather than rates
	};

	const auto date = 2026y / April / 10d;

	const auto& indx = SOFR_compounded_index[date];
	assert(indx);

	// check the latest data available in this example
	cout
		<< fixed
		<< setprecision(SOFR_compounded_index.get_decimal_places())
		<< "For "
		<< date
		<< " SOFR Compounded Index is "
		<< indx->get_value()
		<< " and the same computed value is "
		<< index(SOFR, rfd, date, id).get_value()
		<< endl;

	const auto& _30d_avg = SOFR_30_day_average[date];
	assert(_30d_avg);

	cout
		<< fixed
		<< setprecision(SOFR_30_day_average.get_decimal_places())
		<< "For "
		<< date
		<< " SOFR 30 Day Average is "
		<< _30d_avg->get_value()
		<< " and the same computed value is "
		<< average(SOFR, rfd, date, _30dd).percent.get_value()
		<< endl;

	const auto& _90d_avg = SOFR_90_day_average[date];
	assert(_90d_avg);

	cout
		<< fixed
		<< setprecision(SOFR_90_day_average.get_decimal_places())
		<< "For "
		<< date
		<< " SOFR 90 Day Average is "
		<< _90d_avg->get_value()
		<< " and the same computed value is "
		<< average(SOFR, rfd, date, _90dd).percent.get_value()
		<< endl;

	const auto& _180d_avg = SOFR_180_day_average[date];
	assert(_180d_avg);

	cout
		<< fixed
		<< setprecision(SOFR_180_day_average.get_decimal_places())
		<< "For "
		<< date
		<< " SOFR 180 Day Average is "
		<< _180d_avg->get_value()
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
		cout << "SOFR calendar and SIFMA calendar match" << endl;
	else
	{
		cout << "SOFR calendar and SIFMA calendar do not match" << endl;

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
		cout << "SOFR Compounded Index calendar and SIFMA calendar match" << endl;
	else
	{
		cout << "SOFR Compounded Index calendar and SIFMA calendar do not match" << endl;

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
	const auto period = SOFR_compounded_index.get_time_series().get_period();
	for (
		auto d = period.get_from();
		d <= period.get_until();
		d = sys_days{ d } + days{ 1 }
	)
	{
		const auto& fix = SOFR_compounded_index[d];
		if (fix)
		{
			const auto computed_fix = index(SOFR, rfd, d, id);
			if (*fix != computed_fix)
				cout
					<< fixed
					<< setprecision(SOFR_compounded_index.get_decimal_places())
					<< "For "
					<< d
					<< " SOFR Compounded Index is "
					<< fix->get_value()
					<< " and the same computed value is "
					<< computed_fix.get_value()
					<< endl;
		}
	}

	const auto& SOFR_30_day_average_calendar = SOFR_30_day_average.get_calendar();
	const auto _30_day_dates = SOFR_30_day_average_calendar.make_business_days_schedule(
		SOFR_30_day_average.get_time_series().get_period()
	);
	for (const auto& d : _30_day_dates.get_dates())
	{
		const auto& _30d_avg = SOFR_30_day_average[d];
		assert(_30d_avg);

		const auto computed_avg = average(SOFR, rfd, d, _30dd).percent;
		if (*_30d_avg != computed_avg)
			cout
				<< fixed
				<< setprecision(SOFR_30_day_average.get_decimal_places())
				<< "For "
				<< d
				<< " SOFR 30 Day Average is "
				<< SOFR_30_day_average[d]->get_value()
				<< " and the same computed value is "
				<< computed_avg.get_value()
				<< endl;
	}

	const auto& SOFR_90_day_average_calendar = SOFR_90_day_average.get_calendar();
	const auto _90_day_dates = SOFR_90_day_average_calendar.make_business_days_schedule(
		SOFR_90_day_average.get_time_series().get_period()
	);
	for (const auto& d : _90_day_dates.get_dates())
	{
		const auto& _90d_avg = SOFR_90_day_average[d];
		assert(_90d_avg);

		const auto computed_avg = average(SOFR, rfd, d, _90dd).percent;
		if (*_90d_avg != computed_avg)
			cout
				<< fixed
				<< setprecision(SOFR_90_day_average.get_decimal_places())
				<< "For "
				<< d
				<< " SOFR 90 Day Average is "
				<< SOFR_90_day_average[d]->get_value()
				<< " and the same computed value is "
				<< computed_avg.get_value()
				<< endl;
	}

	const auto& SOFR_180_day_average_calendar = SOFR_180_day_average.get_calendar();
	const auto _180_day_dates = SOFR_180_day_average_calendar.make_business_days_schedule(
		SOFR_180_day_average.get_time_series().get_period()
	);
	for (const auto& d : _180_day_dates.get_dates())
	{
		const auto& _180d_avg = SOFR_180_day_average[d];
		assert(_180d_avg);

		const auto computed_avg = average(SOFR, rfd, d, _180dd).percent;
		if (*_180d_avg != computed_avg)
			cout
				<< fixed
				<< setprecision(SOFR_180_day_average.get_decimal_places())
				<< "For "
				<< d
				<< " SOFR 180 Day Average is "
				<< SOFR_180_day_average[d]->get_value()
				<< " and the same computed value is "
				<< computed_avg .get_value()
				<< endl;
	}

	// interesting case around Good Friday
	const auto rd = rate_detail{
		.start = 2026y / March / 30d,
		.end = 2026y / April / 10d,
		.day_count = actual_360<Decimal>{},
		.round = 5u + 2u // as we deal with fractions, rather than rates
	};
	const auto cd = compounded_detail{
		.calendar = gregorian::static_data::locate_calendar("America/SIFMA", rd.start)
	};
	cout
		<< fixed
		<< setprecision(rd.round) // should be 5 rather than 7
		<< "Compounded SOFR average rate between x and y is "
		<< compounded(SOFR_compounded_index, rd).percent.get_value()
		<< " and the same \"long formula\" rate is "
		<< compounded(SOFR, rfd, cd, rd).percent.get_value()
		<< endl;

	return 0;
}

// do we also need to cosider FedFunds?
