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
#include <optional>

using namespace std;
using namespace std::chrono;

using namespace gregorian;
using namespace gregorian::util;
using namespace gregorian::static_data;

using namespace fin_calendar;

using namespace reset;



static auto make_parser_detail() -> parser_detail
{
	return parser_detail{
		6u,
		2022y / April / 28d,
		2026y / May / 27d,
		"%Y-%m-%d",
		',',
		nullopt,
		nullopt,
		1u
	};
}

static auto parse_csv_fixings_ZARONIA() -> RateFixings
{
	const auto d = make_parser_detail();

	return parse_csv_fixings<RateFixings>(
		"SARB-benchmark-data.csv",
		d,
		"Africa/Johannesburg",
		3u
	);
}

static auto parse_csv_fixings_ZARONIA_compounded_index() -> IndexFixings
{
	auto d = make_parser_detail();
	d.from = 2022y / November / 1d;
	d.not_available = "0.000000000000";
	d.skip_columns = 6u;

	return parse_csv_fixings<IndexFixings>(
		"ZARONIA-Period-Averages-and-Index.csv",
		d,
		"Africa/Johannesburg",
		12u
	);
}



int main()
{
	const auto ZARONIA = parse_csv_fixings_ZARONIA();

	auto rfd = rate_fixing_detail{};
	rfd.day_count = actual_365_fixed<Decimal>{};

	const auto ZARONIA_compounded_index = parse_csv_fixings_ZARONIA_compounded_index();

	// from
	// "Compounded ZARONIA period averages and index
	// Calculation methodology and publication
	// October 2023"
	auto id = index_detail{};
	id.initial_value = Value{ "100" };
	id.initial_date = 2022y / November / 1d;
	id.final_round = 12u;

	const auto date = 2026y / May / 27d;

	const auto& indx = ZARONIA_compounded_index[date];
	assert(indx);
	assert(ZARONIA.get_calendar().is_business_day(date));

	cout
		<< fixed
		<< setprecision(ZARONIA_compounded_index.get_decimal_places())
		<< "For "
		<< date
		<< " ZARONIA Compounded Index is "
		<< indx->get_value()
		<< " and the same computed value is "
		<< index(ZARONIA, rfd, date, id).get_value()
		<< endl;

	return 0;
}
