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



int main()
{
	const auto EuroSTR = parse_csv_fixings_EuroSTR();
	const auto PreEuroSTR = parse_csv_fixings_PreEuroSTR();

	// how can we "add" these 2 together?

	auto rfd = rate_fixing_detail{};
	rfd.day_count = actual_360<Decimal>{};

	const auto EuroSTR_compounded_index = parse_csv_fixings_EuroSTR_compounded_index();

	const auto EuroSTR_1_week_compounded = parse_csv_fixings_EuroSTR_1_week_compounded();

	// from ...
	auto id = index_detail{};
	id.initial_value = Value{ "100" };
	id.initial_date = 2019y / October / 1d;
	id.final_round = 8u;
//	id.step_round = 8u;
	// not 100% sure that above is correct

	auto _1wd = average_detail{};
	_1wd.term = days{ 7 }; // 1 week
	_1wd.business_day_convention = preceding{};
	_1wd.final_round = 5u + 2u; // as we deal with fractions, rather than rates

//	const auto date = 2026y / April / 24d;
	const auto date = 2026y / April / 23d;

	const auto& indx = EuroSTR_compounded_index[date];
	assert(indx);

	cout
		<< fixed
		<< setprecision(EuroSTR_compounded_index.get_decimal_places())
		<< "For "
		<< date
		<< " SOFR Compounded Index is "
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
		<< " EuroST 1 Week Compounded Average is "
		<< _1w_cmp->get_value()
		<< " and the same computed value is "
		<< average(EuroSTR, rfd, date, _1wd).percent.get_value()
		<< endl;

	// look for inconsistencies in the data

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
	return 0;
}
