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
#include <checkers.h>

#include <scaled_value.h>
#include <fixings.h>
#include <index.h>
#include <average.h>
#include <rate.h>
#include <compounded.h>

#include <preceding.h>
#include <modified_preceding.h>
#include <actual_360.h>

#include <boost/decimal.hpp>

#include <chrono>
#include <iostream>
#include <iomanip>
#include <ios>
#include <cassert>
#include <string>

using namespace std;
using namespace std::chrono;

using namespace boost::decimal;
using namespace boost::decimal::literals;

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

	constexpr auto rfd = rate_fixings_detail{
		.day_count = actual_360<decimal128_t>{}
	};

	const auto EuroSTR_compounded_index = parse_csv_fixings_EuroSTR_compounded_index();

	const auto EuroSTR_1_week_compounded = parse_csv_fixings_EuroSTR_1_week_compounded();
	const auto EuroSTR_1_month_compounded = parse_csv_fixings_EuroSTR_1_month_compounded();
	const auto EuroSTR_3_month_compounded = parse_csv_fixings_EuroSTR_3_month_compounded();
	const auto EuroSTR_6_month_compounded = parse_csv_fixings_EuroSTR_6_month_compounded();
	const auto EuroSTR_12_month_compounded = parse_csv_fixings_EuroSTR_12_month_compounded();

	// from
	// "Compounded €STR average rates and index"
	constexpr auto id = index_detail{
		.initial_value = 100_dl,
		.initial_date = 2019y / October / 1d,
		.final_round = 8u
	};

	constexpr auto _1wd = average_detail{
		.term = weeks{ 1 },
		.business_day_convention = preceding{},
		.final_round = 5u + 2u // as we deal with fractions, rather than rates
	};

	constexpr auto _1md = average_detail{
		.term = months{ 1 },
		.business_day_convention = modified_preceding{},
		.final_round = 5u + 2u // as we deal with fractions, rather than rates
	};

	constexpr auto _3md = average_detail{
		.term = months{ 3 },
		.business_day_convention = modified_preceding{},
		.final_round = 5u + 2u // as we deal with fractions, rather than rates
	};

	constexpr auto _6md = average_detail{
		.term = months{ 6 },
		.business_day_convention = modified_preceding{},
		.final_round = 5u + 2u // as we deal with fractions, rather than rates
	};

	constexpr auto _12md = average_detail{
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

	const auto EuroSTR_compounded_index_label = "EuroSTR Compounded Index"s;
	auto EuroSTR_compounded_index_task = make_compounded_index_check_task(
		EuroSTR,
		rfd,
		EuroSTR_compounded_index,
		id,
		EuroSTR_compounded_index_label
	);

	const auto EuroSTR_1_week_compounded_label = "EuroSTR 1 Week Compounded Average"s;
	auto EuroSTR_1_week_compounded_task = make_compounded_average_check_task(
		EuroSTR,
		rfd,
		EuroSTR_1_week_compounded,
		_1wd,
		EuroSTR_1_week_compounded_label
	);

	const auto EuroSTR_1_month_compounded_label = "EuroSTR 1 Month Compounded Average"s;
	auto EuroSTR_1_month_compounded_task = make_compounded_average_check_task(
		EuroSTR,
		rfd,
		EuroSTR_1_month_compounded,
		_1md,
		EuroSTR_1_month_compounded_label
	);

	const auto EuroSTR_3_month_compounded_label = "EuroSTR 3 Month Compounded Average"s;
	auto EuroSTR_3_month_compounded_task = make_compounded_average_check_task(
		EuroSTR,
		rfd,
		EuroSTR_3_month_compounded,
		_3md,
		EuroSTR_3_month_compounded_label
	);

	const auto EuroSTR_6_month_compounded_label = "EuroSTR 6 Month Compounded Average"s;
	auto EuroSTR_6_month_compounded_task = make_compounded_average_check_task(
		EuroSTR,
		rfd,
		EuroSTR_6_month_compounded,
		_6md,
		EuroSTR_6_month_compounded_label
	);

	const auto EuroSTR_12_month_compounded_label = "EuroSTR 12 Month Compounded Average"s;
	auto EuroSTR_12_month_compounded_task = make_compounded_average_check_task(
		EuroSTR,
		rfd,
		EuroSTR_12_month_compounded,
		_12md,
		EuroSTR_12_month_compounded_label
	);

	EuroSTR_compounded_index_task.get();
	EuroSTR_1_week_compounded_task.get();
	EuroSTR_1_month_compounded_task.get();
	EuroSTR_3_month_compounded_task.get();
	EuroSTR_6_month_compounded_task.get();
	EuroSTR_12_month_compounded_task.get();

	constexpr auto rd = rate_detail{
		.start = 2020y / February / 11d,
		.end = 2020y / February / 28d,
		.day_count = actual_360<decimal128_t>{},
		.round = 5u + 2u // as we deal with fractions, rather than rates
	};
	const/*expr*/ auto cd = compounded_detail{
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
