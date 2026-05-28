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

#include <preceding.h>
#include <modified_preceding.h>
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

static auto parse_csv_fixings_ZARONIA_1_week_compounded() -> RateFixings
{
	auto d = make_parser_detail();
	d.skip_columns = 0u;

	return parse_csv_fixings<RateFixings>(
		"ZARONIA-Period-Averages-and-Index.csv",
		d,
		"Africa/Johannesburg",
		5u
	);
}

static auto parse_csv_fixings_ZARONIA_1_month_compounded() -> RateFixings
{
	auto d = make_parser_detail();
	d.skip_columns = 1u;

	return parse_csv_fixings<RateFixings>(
		"ZARONIA-Period-Averages-and-Index.csv",
		d,
		"Africa/Johannesburg",
		5u
	);
}

static auto parse_csv_fixings_ZARONIA_3_month_compounded() -> RateFixings
{
	auto d = make_parser_detail();
	d.skip_columns = 2u;

	return parse_csv_fixings<RateFixings>(
		"ZARONIA-Period-Averages-and-Index.csv",
		d,
		"Africa/Johannesburg",
		5u
	);
}

static auto parse_csv_fixings_ZARONIA_6_month_compounded() -> RateFixings
{
	auto d = make_parser_detail();
	d.skip_columns = 3u;

	return parse_csv_fixings<RateFixings>(
		"ZARONIA-Period-Averages-and-Index.csv",
		d,
		"Africa/Johannesburg",
		5u
	);
}

static auto parse_csv_fixings_ZARONIA_9_month_compounded() -> RateFixings
{
	auto d = make_parser_detail();
	d.skip_columns = 4u;

	return parse_csv_fixings<RateFixings>(
		"ZARONIA-Period-Averages-and-Index.csv",
		d,
		"Africa/Johannesburg",
		5u
	);
}

static auto parse_csv_fixings_ZARONIA_12_month_compounded() -> RateFixings
{
	auto d = make_parser_detail();
	d.skip_columns = 5u;

	return parse_csv_fixings<RateFixings>(
		"ZARONIA-Period-Averages-and-Index.csv",
		d,
		"Africa/Johannesburg",
		5u
	);
}



int main()
{
	const auto ZARONIA = parse_csv_fixings_ZARONIA();

	auto rfd = rate_fixing_detail{};
	rfd.day_count = actual_365_fixed<Decimal>{};

	const auto ZARONIA_compounded_index = parse_csv_fixings_ZARONIA_compounded_index();

	const auto ZARONIA_1_week_compounded = parse_csv_fixings_ZARONIA_1_week_compounded();
	const auto ZARONIA_1_month_compounded = parse_csv_fixings_ZARONIA_1_month_compounded();
	const auto ZARONIA_3_month_compounded = parse_csv_fixings_ZARONIA_3_month_compounded();
	const auto ZARONIA_6_month_compounded = parse_csv_fixings_ZARONIA_6_month_compounded();
	const auto ZARONIA_9_month_compounded = parse_csv_fixings_ZARONIA_9_month_compounded();
	const auto ZARONIA_12_month_compounded = parse_csv_fixings_ZARONIA_12_month_compounded();

	// from
	// "Compounded ZARONIA period averages and index
	// Calculation methodology and publication
	// October 2023"
	auto id = index_detail{};
	id.initial_value = Value{ "100" };
	id.initial_date = 2022y / November / 1d;
	id.final_round = 12u;

	auto _1wd = average_detail{};
	_1wd.term = weeks{ 1 };
	_1wd.business_day_convention = fin_calendar::preceding{};
	_1wd.final_round = 5u + 2u; // as we deal with fractions, rather than rates

	auto _1md = average_detail{};
	_1md.term = months{ 1 };
	_1md.business_day_convention = modified_preceding{};
	_1md.final_round = 5u + 2u; // as we deal with fractions, rather than rates

	auto _3md = average_detail{};
	_3md.term = months{ 3 };
	_3md.business_day_convention = modified_preceding{};
	_3md.final_round = 5u + 2u; // as we deal with fractions, rather than rates

	auto _6md = average_detail{};
	_6md.term = months{ 6 };
	_6md.business_day_convention = modified_preceding{};
	_6md.final_round = 5u + 2u; // as we deal with fractions, rather than rates

	auto _9md = average_detail{};
	_9md.term = months{ 9 };
	_9md.business_day_convention = modified_preceding{};
	_9md.final_round = 5u + 2u; // as we deal with fractions, rather than rates

	auto _12md = average_detail{};
	_12md.term = months{ 12 };
	_12md.business_day_convention = modified_preceding{};
	_12md.final_round = 5u + 2u; // as we deal with fractions, rather than rates

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

	const auto& _1w_cmp = ZARONIA_1_week_compounded[date];
	assert(_1w_cmp);

	cout
		<< fixed
		<< setprecision(ZARONIA_1_week_compounded.get_decimal_places())
		<< "For "
		<< date
		<< " ZARONIA 1 Week Compounded Average is "
		<< _1w_cmp->get_value()
		<< " and the same computed value is "
		<< average(ZARONIA, rfd, date, _1wd).percent.get_value()
		<< endl;

	const auto& _1m_cmp = ZARONIA_1_month_compounded[date];
	assert(_1m_cmp);

	cout
		<< fixed
		<< setprecision(ZARONIA_1_month_compounded.get_decimal_places())
		<< "For "
		<< date
		<< " ZARONIA 1 Month Compounded Average is "
		<< _1m_cmp->get_value()
		<< " and the same computed value is "
		<< average(ZARONIA, rfd, date, _1md).percent.get_value()
		<< endl;

	const auto& _3m_cmp = ZARONIA_3_month_compounded[date];
	assert(_3m_cmp);

	cout
		<< fixed
		<< setprecision(ZARONIA_3_month_compounded.get_decimal_places())
		<< "For "
		<< date
		<< " ZARONIA 3 Month Compounded Average is "
		<< _3m_cmp->get_value()
		<< " and the same computed value is "
		<< average(ZARONIA, rfd, date, _3md).percent.get_value()
		<< endl;

	const auto& _6m_cmp = ZARONIA_6_month_compounded[date];
	assert(_6m_cmp);

	cout
		<< fixed
		<< setprecision(ZARONIA_6_month_compounded.get_decimal_places())
		<< "For "
		<< date
		<< " ZARONIA 6 Month Compounded Average is "
		<< _6m_cmp->get_value()
		<< " and the same computed value is "
		<< average(ZARONIA, rfd, date, _6md).percent.get_value()
		<< endl;

	const auto& _9m_cmp = ZARONIA_9_month_compounded[date];
	assert(_9m_cmp);

	cout
		<< fixed
		<< setprecision(ZARONIA_9_month_compounded.get_decimal_places())
		<< "For "
		<< date
		<< " ZARONIA 9 Month Compounded Average is "
		<< _9m_cmp->get_value()
		<< " and the same computed value is "
		<< average(ZARONIA, rfd, date, _9md).percent.get_value()
		<< endl;

	const auto& _12m_cmp = ZARONIA_12_month_compounded[date];
	assert(_12m_cmp);

	cout
		<< fixed
		<< setprecision(ZARONIA_12_month_compounded.get_decimal_places())
		<< "For "
		<< date
		<< " ZARONIA 12 Month Compounded Average is "
		<< _12m_cmp->get_value()
		<< " and the same computed value is "
		<< average(ZARONIA, rfd, date, _12md).percent.get_value()
		<< endl;

	// look for inconsistencies in the index data

	const auto index_period = ZARONIA_compounded_index.get_time_series().get_period();
	for (
		auto d = index_period.get_from();
		d <= index_period.get_until();
		d = sys_days{ d } + days{ 1 }
	)
	{
		const auto& fix = ZARONIA_compounded_index[d];
		if (fix)
		{
			const auto computed_fix = index(ZARONIA, rfd, d, id);
			if (*fix != computed_fix)
				cout
					<< "For "
					<< d
					<< " ZARONIA Compounded Index is "
					<< fix->get_value()
					<< " and the same computed value is "
					<< computed_fix.get_value()
					<< endl;
		}
	}

	return 0;
}
