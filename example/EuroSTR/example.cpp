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



int main()
{
	const auto EuroSTR = parse_csv_fixings_EuroSTR();
	const auto PreEuroSTR = parse_csv_fixings_PreEuroSTR();

	// how can we "add" these 2 together?

	auto rfd = rate_fixing_detail{};
	rfd.day_count = actual_360<Decimal>{};

	const auto EuroSTR_compounded_index = parse_csv_fixings_EuroSTR_compounded_index();

	// from 
	auto id = index_detail{};
	id.initial_value = Decimal{ 100 };
	id.initial_date = 2019y / October / 1d;
	id.final_round = 8u;
//	id.step_round = 8u;
	// not 100% sure that above is correct

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

	const auto& EuroSTR_compounded_index_calendar = EuroSTR_compounded_index.get_calendar();

	// look for inconsistencies in the data
	const auto index_dates = EuroSTR_compounded_index_calendar.make_business_days_schedule(
		EuroSTR_compounded_index.get_time_series().get_period()
	);
	for (const auto& d : index_dates.get_dates())
	{
		if (d == *index_dates.get_dates().crbegin())
			break;
		// temporary only, unit we sort out start/end of RFR/RFR Index

		const auto& indx = EuroSTR_compounded_index[d];
		assert(indx);

		if (*indx != index(EuroSTR, rfd, d, id))
			cout
			<< fixed
			<< setprecision(EuroSTR_compounded_index.get_decimal_places())
			<< "For "
			<< d
			<< " EuroSTR Compounded Index is "
			<< indx->get_value()
			<< " and the same computed value is "
			<< index(EuroSTR, rfd, d, id).get_value()
			<< endl;
	}

	return 0;
}
