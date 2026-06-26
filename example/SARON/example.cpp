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

#include <scaled_value.h>
#include <fixings.h>
#include <index.h>

#include <actual_360.h>

#include <boost/decimal.hpp>

#include <chrono>
#include <iostream>
#include <iomanip>
#include <ios>
#include <cassert>
#include <optional>

using namespace std;
using namespace std::chrono;

using namespace boost::decimal;
using namespace boost::decimal::literals;

using namespace fin_calendar;

using namespace reset;



static auto parse_csv_fixings_SARON() -> RateFixings
{
	constexpr auto d = parser_detail{
		.header_lines = 4u,
		.from = 1999y / June / 30d,
		.until = 2026y / May / 12d,
		.date_format = "%d.%m.%Y",
		.separator = ';',
		.padder = ' ',
		.not_available = nullopt,
		.skip_columns = 0u
	};

	return parse_csv_fixings<RateFixings>(
		"hsrron.csv",
		d,
		"Europe/Zurich",
		6u
	);
}
// I think SARON is published several times a day
// but for SAION we need to know the value at the end of the day (18:00)

static auto parse_csv_fixings_current_rate() -> RateFixings
{
	constexpr auto d = parser_detail{
		.header_lines = 4u,
		.from = 1999y / June / 30d,
		.until = 2026y / May / 12d,
		.date_format = "%d.%m.%Y",
		.separator = ';',
		.padder = ' ',
		.not_available = nullopt,
		.skip_columns = 3u
	};

	return parse_csv_fixings<RateFixings>(
		"hsrron.csv",
		d,
		"Europe/Zurich",
		6u
	);
}

static auto parse_csv_fixings_SAION() -> IndexFixings
{
	constexpr auto d = parser_detail{
		.header_lines = 4u,
		.from = 1999y / June / 30d,
		.until = 2026y / May / 12d,
		.date_format = "%d.%m.%Y",
		.separator = ';',
		.padder = ' ',
		.not_available = nullopt,
		.skip_columns = 4u
	};

	return parse_csv_fixings<IndexFixings>(
		"hsrron.csv",
		d,
		"Europe/Zurich",
		6u
	);
}

static auto parse_csv_fixings_current_index() -> IndexFixings
{
	constexpr auto d = parser_detail{
		.header_lines = 4u,
		.from = 1999y / June / 30d,
		.until = 2026y / May / 12d,
		.date_format = "%d.%m.%Y",
		.separator = ';',
		.padder = ' ',
		.not_available = nullopt,
		.skip_columns = 5u
	};

	return parse_csv_fixings<IndexFixings>(
		"hsrron.csv",
		d,
		"Europe/Zurich",
		6u
	);
}



int main()
{
	// from https://indexdata.six-group.com/swiss_reference_rates/reference_rates.html

	const auto SARON = parse_csv_fixings_SARON();
	const auto current_rate = parse_csv_fixings_current_rate();
	const auto SAION = parse_csv_fixings_SAION();
	const auto current_index = parse_csv_fixings_current_index();
	// we can assert consistency between what was read above

	constexpr auto rfd = rate_fixings_detail{
		.day_count = actual_360<decimal128_t>{}
	};

	// from https://indexdata.six-group.com/download/online/vendor_code/six-calculated-indices.xls

	constexpr auto id = index_detail{
		.initial_value = 10000_dl,
		.initial_date = 1999y / June / 30d,
		.step_round = 6u
	};

	const auto& date = SARON.get_time_series().get_period().get_until();

	const auto& indx = SAION[date];
	assert(indx);

	cout
		<< fixed
		<< setprecision(SAION.get_decimal_places())
		<< "For "
		<< date
		<< " SAION is "
		<< indx->get_value()
		<< " and the same computed value is "
		<< index(SARON, rfd, date, id).get_value()
		<< endl;

	const auto& current_indx = current_index[date];
	assert(current_indx);

	cout
		<< fixed
		<< setprecision(current_index.get_decimal_places())
		<< "For "
		<< date
		<< " Current Index is "
		<< current_indx->get_value()
		<< " and the same computed value is "
		<< index(current_rate, rfd, date, id).get_value()
		<< endl;

	return 0;
}
