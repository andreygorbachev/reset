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
#include <rate.h>
#include <compounded.h>

#include <preceding.h>
#include <modified_preceding.h>
#include <actual_360.h>

#include <chrono>
#include <iostream>
#include <iomanip>
#include <ios>
#include <cassert>

using namespace std;
using namespace std::chrono;

using namespace fin_calendar;

using namespace reset;



static auto parse_csv_fixings_EuroSTR() -> RateFixings
{
	// from https://www.ecb.europa.eu/stats/financial_markets_and_interest_rates/euro_short-term_rate/html/index.en.html
	return parse_csv_fixings<RateFixings>(
		"Euro short-term rate.csv",
		1u,
		2019y / October / 1d,
		2026y / April / 23d,
		3u
	);
}

static auto parse_csv_fixings_PreEuroSTR() -> RateFixings
{
	// from https://www.ecb.europa.eu/stats/financial_markets_and_interest_rates/euro_short-term_rate/html/index.en.html
	return parse_csv_fixings<RateFixings>(
		"Pre-Euro short-term rate.csv",
		1u,
		2017y / March / 15d,
		2019y / September / 30d,
		3u
	);
}

static auto parse_csv_fixings_EuroSTR_compounded_index() -> IndexFixings
{
	// from https://www.ecb.europa.eu/stats/financial_markets_and_interest_rates/euro_short-term_rate/html/index.en.html
	return parse_csv_fixings<IndexFixings>(
		"Compounded euro-short term rates and index.csv",
		1u,
		2019y / October / 1d,
		2026y / April / 24d,
		8u
	);
}

static auto parse_csv_fixings_EuroSTR_1_week_compounded() -> RateFixings
{
	return parse_csv_fixings<RateFixings>(
		"Compounded euro-short term rates and index.csv",
		2u,
		2019y / October / 8d,
		2026y / April / 24d,
		5u
	);
}

static auto parse_csv_fixings_EuroSTR_1_month_compounded() -> RateFixings
{
	return parse_csv_fixings<RateFixings>(
		"Compounded euro-short term rates and index.csv",
		3u,
		2019y / November / 1d,
		2026y / April / 24d,
		5u
	);
}

static auto parse_csv_fixings_EuroSTR_3_month_compounded() -> RateFixings
{
	return parse_csv_fixings<RateFixings>(
		"Compounded euro-short term rates and index.csv",
		4u,
		2020y / January / 2d,
		2026y / April / 24d,
		5u
	);
}

static auto parse_csv_fixings_EuroSTR_6_month_compounded() -> RateFixings
{
	return parse_csv_fixings<RateFixings>(
		"Compounded euro-short term rates and index.csv",
		5u,
		2020y / April / 1d,
		2026y / April / 24d,
		5u
	);
}

static auto parse_csv_fixings_EuroSTR_12_month_compounded() -> RateFixings
{
	return parse_csv_fixings<RateFixings>(
		"Compounded euro-short term rates and index.csv",
		6u,
		2020y / October / 1d,
		2026y / April / 24d,
		5u
	);
}



int main()
{
	const auto EuroSTR = parse_csv_fixings_EuroSTR();
	const auto PreEuroSTR = parse_csv_fixings_PreEuroSTR();
	// how can we "add" these 2 together?

	const auto rfd = rate_fixings_detail{
		.day_count = actual_360<Decimal>{}
	};

	const auto EuroSTR_compounded_index = parse_csv_fixings_EuroSTR_compounded_index();

	const auto EuroSTR_1_week_compounded = parse_csv_fixings_EuroSTR_1_week_compounded();
	const auto EuroSTR_1_month_compounded = parse_csv_fixings_EuroSTR_1_month_compounded();
	const auto EuroSTR_3_month_compounded = parse_csv_fixings_EuroSTR_3_month_compounded();
	const auto EuroSTR_6_month_compounded = parse_csv_fixings_EuroSTR_6_month_compounded();
	const auto EuroSTR_12_month_compounded = parse_csv_fixings_EuroSTR_12_month_compounded();

	// from
	// "Compounded €STR average rates and index"
	const auto id = index_detail{
		.initial_value = Value{ "100" },
		.initial_date = 2019y / October / 1d,
		.final_round = 8u
	};

	const auto _1wd = average_detail{
		.term = weeks{ 1 },
		.business_day_convention = preceding{},
		.final_round = 5u + 2u // as we deal with fractions, rather than rates
	};

	const auto _1md = average_detail{
		.term = months{ 1 },
		.business_day_convention = modified_preceding{},
		.final_round = 5u + 2u // as we deal with fractions, rather than rates
	};

	const auto _3md = average_detail{
		.term = months{ 3 },
		.business_day_convention = modified_preceding{},
		.final_round = 5u + 2u // as we deal with fractions, rather than rates
	};

	const auto _6md = average_detail{
		.term = months{ 6 },
		.business_day_convention = modified_preceding{},
		.final_round = 5u + 2u // as we deal with fractions, rather than rates
	};

	const auto _12md = average_detail{
		.term = months{ 12 },
		.business_day_convention = modified_preceding{},
		.final_round = 5u + 2u // as we deal with fractions, rather than rates
	};

	const auto date = 2026y / April / 24d;

	const auto& indx = EuroSTR_compounded_index[date];
	assert(indx);

	cout
		<< fixed
		<< setprecision(EuroSTR_compounded_index.get_decimal_places())
		<< "For "
		<< date
		<< " EuroSTR Compounded Index is "
		<< indx->get_value()
		<< " and the same computed value is "
		<< index(EuroSTR, rfd, date, id).get_value()
		<< endl;

	const auto& _1w_cmp = EuroSTR_1_week_compounded[date];
	assert(_1w_cmp);

	cout
		<< fixed
		<< setprecision(EuroSTR_1_week_compounded.get_decimal_places())
		<< "For "
		<< date
		<< " EuroSTR 1 Week Compounded Average is "
		<< _1w_cmp->get_value()
		<< " and the same computed value is "
		<< average(EuroSTR, rfd, date, _1wd).percent.get_value()
		<< endl;

	const auto& _1m_cmp = EuroSTR_1_month_compounded[date];
	assert(_1m_cmp);

	cout
		<< fixed
		<< setprecision(EuroSTR_1_month_compounded.get_decimal_places())
		<< "For "
		<< date
		<< " EuroSTR 1 Month Compounded Average is "
		<< _1m_cmp->get_value()
		<< " and the same computed value is "
		<< average(EuroSTR, rfd, date, _1md).percent.get_value()
		<< endl;

	const auto& _3m_cmp = EuroSTR_3_month_compounded[date];
	assert(_3m_cmp);

	cout
		<< fixed
		<< setprecision(EuroSTR_3_month_compounded.get_decimal_places())
		<< "For "
		<< date
		<< " EuroSTR 3 Month Compounded Average is "
		<< _3m_cmp->get_value()
		<< " and the same computed value is "
		<< average(EuroSTR, rfd, date, _3md).percent.get_value()
		<< endl;

	const auto& _6m_cmp = EuroSTR_6_month_compounded[date];
	assert(_6m_cmp);

	cout
		<< fixed
		<< setprecision(EuroSTR_6_month_compounded.get_decimal_places())
		<< "For "
		<< date
		<< " EuroSTR 6 Month Compounded Average is "
		<< _6m_cmp->get_value()
		<< " and the same computed value is "
		<< average(EuroSTR, rfd, date, _6md).percent.get_value()
		<< endl;

	const auto& _12m_cmp = EuroSTR_12_month_compounded[date];
	assert(_12m_cmp);

	cout
		<< fixed
		<< setprecision(EuroSTR_12_month_compounded.get_decimal_places())
		<< "For "
		<< date
		<< " EuroSTR 12 Month Compounded Average is "
		<< _12m_cmp->get_value()
		<< " and the same computed value is "
		<< average(EuroSTR, rfd, date, _12md).percent.get_value()
		<< endl;

	// look for inconsistencies in the data

	const auto period = EuroSTR_compounded_index.get_time_series().get_period();
	for (
		auto d = period.get_from();
		d <= period.get_until();
		d = sys_days{ d } + days{ 1 }
	)
	{
		const auto& fix = EuroSTR_compounded_index[d];
		if (fix)
		{
			const auto computed_fix = index(EuroSTR, rfd, d, id);
			if (*fix != computed_fix)
				cout
					<< "For "
					<< d
					<< " EuroSTR Compounded Index is "
					<< fix->get_value()
					<< " and the same computed value is "
					<< computed_fix.get_value()
					<< endl;
		}
	}

	const auto& EuroSTR_1_week_compounded_calendar = EuroSTR_1_week_compounded.get_calendar();
	const auto _1_week_dates = EuroSTR_1_week_compounded_calendar.make_business_days_schedule(
		EuroSTR_1_week_compounded.get_time_series().get_period()
	);
	for (const auto& d : _1_week_dates.get_dates())
	{
		const auto& _1w_avg = EuroSTR_1_week_compounded[d];
		assert(_1w_avg);

		if (*_1w_avg != average(EuroSTR, rfd, d, _1wd).percent)
			cout
				<< fixed
				<< setprecision(EuroSTR_1_week_compounded.get_decimal_places())
				<< "For "
				<< d
				<< " EuroSTR 1 Week Compounded Average is "
				<< EuroSTR_1_week_compounded[d]->get_value()
				<< " and the same computed value is "
				<< average(EuroSTR, rfd, d, _1wd).percent.get_value()
				<< endl;
	}

	const auto& EuroSTR_1_month_compounded_calendar = EuroSTR_1_month_compounded.get_calendar();
	const auto _1_month_dates = EuroSTR_1_month_compounded_calendar.make_business_days_schedule(
		EuroSTR_1_month_compounded.get_time_series().get_period()
	);
	for (const auto& d : _1_month_dates.get_dates())
	{
		const auto& _1m_avg = EuroSTR_1_month_compounded[d];
		assert(_1m_avg);

		if (*_1m_avg != average(EuroSTR, rfd, d, _1md).percent)
			cout
				<< fixed
				<< setprecision(EuroSTR_1_month_compounded.get_decimal_places())
				<< "For "
				<< d
				<< " EuroSTR 1 Month Compounded Average is "
				<< EuroSTR_1_month_compounded[d]->get_value()
				<< " and the same computed value is "
				<< average(EuroSTR, rfd, d, _1md).percent.get_value()
				<< endl;
	}

	const auto& EuroSTR_3_month_compounded_calendar = EuroSTR_3_month_compounded.get_calendar();
	const auto _3_month_dates = EuroSTR_3_month_compounded_calendar.make_business_days_schedule(
		EuroSTR_3_month_compounded.get_time_series().get_period()
	);
	for (const auto& d : _3_month_dates.get_dates())
	{
		const auto& _3m_avg = EuroSTR_3_month_compounded[d];
		assert(_3m_avg);

		if (*_3m_avg != average(EuroSTR, rfd, d, _3md).percent)
			cout
				<< fixed
				<< setprecision(EuroSTR_3_month_compounded.get_decimal_places())
				<< "For "
				<< d
				<< " EuroSTR 3 Month Compounded Average is "
				<< EuroSTR_3_month_compounded[d]->get_value()
				<< " and the same computed value is "
				<< average(EuroSTR, rfd, d, _3md).percent.get_value()
				<< endl;
	}

	const auto& EuroSTR_6_month_compounded_calendar = EuroSTR_6_month_compounded.get_calendar();
	const auto _6_month_dates = EuroSTR_6_month_compounded_calendar.make_business_days_schedule(
		EuroSTR_6_month_compounded.get_time_series().get_period()
	);
	for (const auto& d : _6_month_dates.get_dates())
	{
		const auto& _6m_avg = EuroSTR_6_month_compounded[d];
		assert(_6m_avg);

		if (*_6m_avg != average(EuroSTR, rfd, d, _6md).percent)
			cout
				<< fixed
				<< setprecision(EuroSTR_6_month_compounded.get_decimal_places())
				<< "For "
				<< d
				<< " EuroSTR 6 Month Compounded Average is "
				<< EuroSTR_6_month_compounded[d]->get_value()
				<< " and the same computed value is "
				<< average(EuroSTR, rfd, d, _6md).percent.get_value()
				<< endl;
	}

	const auto& EuroSTR_12_month_compounded_calendar = EuroSTR_12_month_compounded.get_calendar();
	const auto _12_month_dates = EuroSTR_12_month_compounded_calendar.make_business_days_schedule(
		EuroSTR_12_month_compounded.get_time_series().get_period()
	);
	for (const auto& d : _12_month_dates.get_dates())
	{
		const auto& _12m_avg = EuroSTR_12_month_compounded[d];
		assert(_12m_avg);

		if (*_12m_avg != average(EuroSTR, rfd, d, _12md).percent)
			cout
				<< fixed
				<< setprecision(EuroSTR_12_month_compounded.get_decimal_places())
				<< "For "
				<< d
				<< " EuroSTR 12 Month Compounded Average is "
				<< EuroSTR_12_month_compounded[d]->get_value()
				<< " and the same computed value is "
				<< average(EuroSTR, rfd, d, _12md).percent.get_value()
				<< endl;
	}

	const auto rd = rate_detail{
		.start = 2020y / February / 11d,
		.end = 2020y / February / 28d,
		.day_count = actual_360<Decimal>{},
		.round = 5u + 2u // as we deal with fractions, rather than rates
	};
	const auto cd = compounded_detail{
		.calendar = EuroSTR.get_calendar()
	};
	cout
		<< fixed
		<< setprecision(rd.round) // should be 5 rather than 7
		<< "Compounded EuroSTR average rate between x and y is "
		<< compounded(EuroSTR_compounded_index, rd).percent.get_value()
		<< " and the same \"long formula\" rate is "
		<< compounded(EuroSTR, rfd, cd, rd).percent.get_value()
		<< endl;

	return 0;
}
