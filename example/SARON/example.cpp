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

#include <actual_360.h>

#include <chrono>
#include <iostream>
#include <iomanip>
#include <ios>
#include <cassert>
#include <utility>

using namespace std;
using namespace std::chrono;

using namespace fin_calendar;

using namespace reset;



static auto parse_csv_fixings_SARON_and_SARON_compounded_index() -> pair<RateFixings, IndexFixings>
{
	return parse_csv_fixings_x2(
		"saron_compound_calculator.csv",
		1999y / June / 30d,
		2020y / February / 17d
	);
}



int main()
{
	// from saron_compound_calculator.xls from SIX

	const auto [SARON, SARON_compounded_index] = parse_csv_fixings_SARON_and_SARON_compounded_index();

	auto rfd = rate_fixing_detail{};
	rfd.day_count = actual_360<Decimal>{};

	auto id = index_detail{};
	id.initial_value = Value{ "10000" };
	id.initial_date = 1999y / June / 30d;
	id.final_round = 4u;

//	const auto date = 2020y / February / 17d;
	const auto date = 2020y / January / 24d; // then things don't work

	const auto& indx = SARON_compounded_index[date];
	assert(indx);

	cout
		<< fixed
		<< setprecision(SARON_compounded_index.get_decimal_places())
		<< "For "
		<< date
		<< " SARON Compounded Index is "
		<< indx->get_value()
		<< " and the same computed value is "
		<< index(SARON, rfd, date, id).get_value();

	return 0;
}
