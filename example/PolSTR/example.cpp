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
#include <average.h>

#include <modified_preceding.h>
#include <actual_365_fixed.h>

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



// from https://gpwbenchmark.pl/delayed_data
// and https://gpwbenchmark.pl/historical_data

static constexpr auto make_parser_detail() -> parser_detail
{
	return parser_detail{
		.header_lines = 1u,
		.from = 2021y / January / 4d,
		.until = 2026y / May / 4d,
		.date_format = "%Y-%m-%d",
		.separator = ',',
		.padder = nullopt,
		.not_available = nullopt,
		.skip_columns = 0u
	};
}

static auto parse_csv_fixings_PolSTR() -> RateFixings
{
	constexpr auto d = make_parser_detail();

	return parse_csv_fixings<RateFixings>(
		"PolSTR.csv",
		d,
		"Europe/Warsaw",
		3u
	);
}

static auto parse_csv_fixings_PolSTR_compounded_index() -> IndexFixings
{
	auto d = make_parser_detail();
	d.until = 2026y / May / 5d;
	d.skip_columns = 4u;

	return parse_csv_fixings<IndexFixings>(
		"PolSTR.csv",
		d,
		"Europe/Warsaw",
		8u
	);
}

static auto parse_csv_fixings_PolSTR_1_month_compounded() -> RateFixings
{
	auto d = make_parser_detail();
	d.from = 2021y / February / 1d;
	d.until = 2026y / May / 5d;
	d.skip_columns = 1u;

	return parse_csv_fixings<RateFixings>(
		"PolSTR.csv",
		d,
		"Europe/Warsaw",
		5u
	);
}

static auto parse_csv_fixings_PolSTR_3_month_compounded() -> RateFixings
{
	auto d = make_parser_detail();
	d.from = 2021y / April / 1d;
	d.until = 2026y / May / 5d;
	d.skip_columns = 2u;

	return parse_csv_fixings<RateFixings>(
		"PolSTR.csv",
		d,
		"Europe/Warsaw",
		5u
	);
}

static auto parse_csv_fixings_PolSTR_6_month_compounded() -> RateFixings
{
	auto d = make_parser_detail();
	d.from = 2021y / July / 1d;
	d.until = 2026y / May / 5d;
	d.skip_columns = 3u;

	return parse_csv_fixings<RateFixings>(
		"PolSTR.csv",
		d,
		"Europe/Warsaw",
		5u
	);
}



int main()
{
	const auto PolSTR = parse_csv_fixings_PolSTR();

	constexpr auto rfd = rate_fixings_detail{
		.day_count = actual_365_fixed<decimal128_t>{}
	};

	const auto PolSTR_compounded_index = parse_csv_fixings_PolSTR_compounded_index();

	const auto PolSTR_1_month_compounded = parse_csv_fixings_PolSTR_1_month_compounded();
	const auto PolSTR_3_month_compounded = parse_csv_fixings_PolSTR_3_month_compounded();
	const auto PolSTR_6_month_compounded = parse_csv_fixings_PolSTR_6_month_compounded();

	// from https://gpwbenchmark.pl/documentation-transaction-based
	constexpr auto id = index_detail{
		.initial_value = 100_DL,
		.initial_date = 2021y / January / 4d,
		.final_round = 8u
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

	const auto date = PolSTR_compounded_index.get_time_series().get_period().get_until();

	const auto& indx = PolSTR_compounded_index[date];
	assert(indx);

	cout
		<< fixed
		<< setprecision(PolSTR_compounded_index.get_decimal_places())
		<< "For "
		<< date
		<< " PolSTR Compounded Index is "
		<< indx->get_value()
		<< " and the same computed value is "
		<< index(PolSTR, rfd, date, id).get_value()
		<< endl;

	const auto& _1m_cmp = PolSTR_1_month_compounded[date];
	assert(_1m_cmp);

	cout
		<< fixed
		<< setprecision(PolSTR_1_month_compounded.get_decimal_places())
		<< "For "
		<< date
		<< " PolSTR 1 Month Compounded Average is "
		<< _1m_cmp->get_value()
		<< " and the same computed value is "
		<< average(PolSTR, rfd, date, _1md).percent.get_value()
		<< endl;

	const auto& _3m_cmp = PolSTR_3_month_compounded[date];
	assert(_3m_cmp);

	cout
		<< fixed
		<< setprecision(PolSTR_3_month_compounded.get_decimal_places())
		<< "For "
		<< date
		<< " PolSTR 3 Month Compounded Average is "
		<< _3m_cmp->get_value()
		<< " and the same computed value is "
		<< average(PolSTR, rfd, date, _3md).percent.get_value()
		<< endl;

	const auto& _6m_cmp = PolSTR_6_month_compounded[date];
	assert(_6m_cmp);

	cout
		<< fixed
		<< setprecision(PolSTR_6_month_compounded.get_decimal_places())
		<< "For "
		<< date
		<< " PolSTR 6 Month Compounded Average is "
		<< _6m_cmp->get_value()
		<< " and the same computed value is "
		<< average(PolSTR, rfd, date, _6md).percent.get_value()
		<< endl;

	// look for inconsistencies in the data

	const auto period = PolSTR_compounded_index.get_time_series().get_period();
	for (
		auto d = period.get_from();
		d <= period.get_until();
		d = sys_days{ d } + days{ 1 }
	)
	{
		const auto& fix = PolSTR_compounded_index[d];
		if (fix)
		{
			const auto computed_fix = index(PolSTR, rfd, d, id);
			if (*fix != computed_fix)
				cout
					<< fixed
					<< setprecision(PolSTR_compounded_index.get_decimal_places())
					<< "For "
					<< d
					<< " PolSTR Compounded Index is "
					<< fix->get_value()
					<< " and the same computed value is "
					<< computed_fix.get_value()
					<< endl;
		}
	}

	const auto& PolSTR_1_month_compounded_calendar = PolSTR_1_month_compounded.get_calendar();
	const auto _1_month_dates = PolSTR_1_month_compounded_calendar.make_business_days_schedule(
		PolSTR_1_month_compounded.get_time_series().get_period()
	);
	for (const auto& d : _1_month_dates.get_dates())
	{
		const auto& _1m_avg = PolSTR_1_month_compounded[d];
		assert(_1m_avg);

		if (*_1m_avg != average(PolSTR, rfd, d, _1md).percent)
			cout
				<< fixed
				<< setprecision(PolSTR_1_month_compounded.get_decimal_places())
				<< "For "
				<< d
				<< " PolSTR 1 Month Compounded Average is "
				<< PolSTR_1_month_compounded[d]->get_value()
				<< " and the same computed value is "
				<< average(PolSTR, rfd, d, _1md).percent.get_value()
				<< endl;
	}

	const auto& PolSTR_3_month_compounded_calendar = PolSTR_3_month_compounded.get_calendar();
	const auto _3_month_dates = PolSTR_3_month_compounded_calendar.make_business_days_schedule(
		PolSTR_3_month_compounded.get_time_series().get_period()
	);
	for (const auto& d : _3_month_dates.get_dates())
	{
		const auto& _3m_avg = PolSTR_3_month_compounded[d];
		assert(_3m_avg);

		if (*_3m_avg != average(PolSTR, rfd, d, _3md).percent)
			cout
				<< fixed
				<< setprecision(PolSTR_3_month_compounded.get_decimal_places())
				<< "For "
				<< d
				<< " PolSTR 3 Month Compounded Average is "
				<< PolSTR_3_month_compounded[d]->get_value()
				<< " and the same computed value is "
				<< average(PolSTR, rfd, d, _3md).percent.get_value()
				<< endl;
	}

	const auto& PolSTR_6_month_compounded_calendar = PolSTR_6_month_compounded.get_calendar();
	const auto _6_month_dates = PolSTR_6_month_compounded_calendar.make_business_days_schedule(
		PolSTR_6_month_compounded.get_time_series().get_period()
	);
	for (const auto& d : _6_month_dates.get_dates())
	{
		const auto& _6m_avg = PolSTR_6_month_compounded[d];
		assert(_6m_avg);

		if (*_6m_avg != average(PolSTR, rfd, d, _6md).percent)
			cout
				<< fixed
				<< setprecision(PolSTR_6_month_compounded.get_decimal_places())
				<< "For "
				<< d
				<< " PolSTR 6 Month Compounded Average is "
				<< PolSTR_6_month_compounded[d]->get_value()
				<< " and the same computed value is "
				<< average(PolSTR, rfd, d, _6md).percent.get_value()
				<< endl;
	}

	return 0;
}
