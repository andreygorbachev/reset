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
#include <rate.h>
#include <average.h>

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
		.until = 2026y / July / 2d,
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
		.until = 2026y / July / 2d,
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
		.until = 2026y / July / 2d,
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
		.until = 2026y / July / 2d,
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

static auto parse_csv_fixings_SARON_1_week_compounded() -> RateFixings
{
	constexpr auto d = parser_detail{
		.header_lines = 1u,
		.from = 2000y / June / 29d,
		.until = 2026y / July / 2d,
		.date_format = "%d.%m.%Y",
		.separator = ';',
		.padder = nullopt,
		.not_available = nullopt,
		.skip_columns = 3u
	};

	return parse_csv_fixings<RateFixings>(
		"h_sar1wc_delayed.csv",
		d,
		"Europe/Zurich",
		4u
	);
}

static auto parse_csv_fixings_SARON_1_month_compounded() -> RateFixings
{
	constexpr auto d = parser_detail{
		.header_lines = 1u,
		.from = 2000y / June / 29d,
		.until = 2026y / July / 2d,
		.date_format = "%d.%m.%Y",
		.separator = ';',
		.padder = nullopt,
		.not_available = nullopt,
		.skip_columns = 3u
	};

	return parse_csv_fixings<RateFixings>(
		"h_sar1mc_delayed.csv",
		d,
		"Europe/Zurich",
		4u
	);
}

static auto parse_csv_fixings_SARON_2_month_compounded() -> RateFixings
{
	constexpr auto d = parser_detail{
		.header_lines = 1u,
		.from = 2000y / June / 29d,
		.until = 2026y / July / 2d,
		.date_format = "%d.%m.%Y",
		.separator = ';',
		.padder = nullopt,
		.not_available = nullopt,
		.skip_columns = 3u
	};

	return parse_csv_fixings<RateFixings>(
		"h_sar2mc_delayed.csv",
		d,
		"Europe/Zurich",
		4u
	);
}

static auto parse_csv_fixings_SARON_3_month_compounded() -> RateFixings
{
	constexpr auto d = parser_detail{
		.header_lines = 1u,
		.from = 2000y / June / 29d,
		.until = 2026y / July / 2d,
		.date_format = "%d.%m.%Y",
		.separator = ';',
		.padder = nullopt,
		.not_available = nullopt,
		.skip_columns = 3u
	};

	return parse_csv_fixings<RateFixings>(
		"h_sar3mc_delayed.csv",
		d,
		"Europe/Zurich",
		4u
	);
}

static auto parse_csv_fixings_SARON_6_month_compounded() -> RateFixings
{
	constexpr auto d = parser_detail{
		.header_lines = 1u,
		.from = 2000y / June / 29d,
		.until = 2026y / July / 2d,
		.date_format = "%d.%m.%Y",
		.separator = ';',
		.padder = nullopt,
		.not_available = nullopt,
		.skip_columns = 3u
	};

	return parse_csv_fixings<RateFixings>(
		"h_sar6mc_delayed.csv",
		d,
		"Europe/Zurich",
		4u
	);
}

static auto parse_csv_fixings_SARON_9_month_compounded() -> RateFixings
{
	constexpr auto d = parser_detail{
		.header_lines = 1u,
		.from = 2000y / June / 29d,
		.until = 2026y / July / 2d,
		.date_format = "%d.%m.%Y",
		.separator = ';',
		.padder = nullopt,
		.not_available = nullopt,
		.skip_columns = 3u
	};

	return parse_csv_fixings<RateFixings>(
		"h_sar9mc_delayed.csv",
		d,
		"Europe/Zurich",
		4u
	);
}

static auto parse_csv_fixings_SARON_12_month_compounded() -> RateFixings
{
	constexpr auto d = parser_detail{
		.header_lines = 1u,
		.from = 2000y / June / 29d,
		.until = 2026y / July / 2d,
		.date_format = "%d.%m.%Y",
		.separator = ';',
		.padder = nullopt,
		.not_available = nullopt,
		.skip_columns = 3u
	};

	return parse_csv_fixings<RateFixings>(
		"h_sar12mc_delayed.csv",
		d,
		"Europe/Zurich",
		4u
	);
}



static auto SARON_average(
	const RateFixings& fix,
	const rate_fixings_detail& rfd,
	const std::chrono::year_month_day& ymd,
	const average_detail& detail = average_detail{} // does it need a default?
) -> rate
{
	// we effectively ignore average_detail.business_day_convention, which is not clean
	// should SARON's calculation of the start date be implemented as another business day convention?

	return average(fix, rfd, ymd, detail); // temp
}




int main()
{
	// from https://indexdata.six-group.com/swiss_reference_rates/reference_rates.html

	const auto SARON = parse_csv_fixings_SARON();
	const auto current_rate = parse_csv_fixings_current_rate();
	const auto SAION = parse_csv_fixings_SAION();
	const auto current_index = parse_csv_fixings_current_index();
	// we can assert consistency between what was read above

	// from https://indexdata.six-group.com/swiss_reference_rates/compound_rates.html

	const auto SARON_1_week_compounded = parse_csv_fixings_SARON_1_week_compounded();
	const auto SARON_1_month_compounded = parse_csv_fixings_SARON_1_month_compounded();
	const auto SARON_2_month_compounded = parse_csv_fixings_SARON_2_month_compounded();
	const auto SARON_3_month_compounded = parse_csv_fixings_SARON_3_month_compounded();
	const auto SARON_6_month_compounded = parse_csv_fixings_SARON_6_month_compounded();
	const auto SARON_9_month_compounded = parse_csv_fixings_SARON_9_month_compounded();
	const auto SARON_12_month_compounded = parse_csv_fixings_SARON_12_month_compounded();
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

	constexpr auto _1wd = average_detail{
		.term = weeks{ 1 },
		.business_day_convention = preceding{}, // temp only
		.final_round = 4u + 2u // as we deal with fractions, rather than rates
	};

	constexpr auto _1md = average_detail{
		.term = months{ 1 },
		.business_day_convention = preceding{}, // temp only
		.final_round = 4u + 2u // as we deal with fractions, rather than rates
	};

	constexpr auto _2md = average_detail{
		.term = months{ 2 },
		.business_day_convention = preceding{}, // temp only
		.final_round = 4u + 2u // as we deal with fractions, rather than rates
	};

	constexpr auto _3md = average_detail{
		.term = months{ 3 },
		.business_day_convention = preceding{}, // temp only
		.final_round = 4u + 2u // as we deal with fractions, rather than rates
	};

	constexpr auto _6md = average_detail{
		.term = months{ 6 },
		.business_day_convention = preceding{}, // temp only
		.final_round = 4u + 2u // as we deal with fractions, rather than rates
	};

	constexpr auto _9md = average_detail{
		.term = months{ 9 },
		.business_day_convention = preceding{}, // temp only
		.final_round = 4u + 2u // as we deal with fractions, rather than rates
	};

	constexpr auto _12md = average_detail{
		.term = months{ 12 },
		.business_day_convention = preceding{}, // temp only
		.final_round = 4u + 2u // as we deal with fractions, rather than rates
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

	const auto avg_date = SARON.get_calendar().shift_business_days(date, days{ 1 });

	const auto& _1w_cmp = SARON_1_week_compounded[date];
	assert(_1w_cmp);

	cout
		<< fixed
		<< setprecision(SARON_1_week_compounded.get_decimal_places())
		<< "For "
		<< avg_date
		<< " SARON 1 Week Compounded Average is "
		<< _1w_cmp->get_value()
		<< " and the same computed value is "
		<< SARON_average(SARON, rfd, avg_date, _1wd).percent.get_value()
		<< endl;

	const auto& _1m_cmp = SARON_1_month_compounded[date];
	assert(_1m_cmp);

	cout
		<< fixed
		<< setprecision(SARON_1_month_compounded.get_decimal_places())
		<< "For "
		<< avg_date
		<< " SARON 1 Month Compounded Average is "
		<< _1m_cmp->get_value()
		<< " and the same computed value is "
		<< SARON_average(SARON, rfd, avg_date, _1md).percent.get_value()
		<< endl;

	const auto& _2m_cmp = SARON_2_month_compounded[date];
	assert(_2m_cmp);

	cout
		<< fixed
		<< setprecision(SARON_2_month_compounded.get_decimal_places())
		<< "For "
		<< avg_date
		<< " SARON 2 Month Compounded Average is "
		<< _2m_cmp->get_value()
		<< " and the same computed value is "
		<< SARON_average(SARON, rfd, avg_date, _2md).percent.get_value()
		<< endl;

	const auto& _3m_cmp = SARON_3_month_compounded[date];
	assert(_3m_cmp);

	cout
		<< fixed
		<< setprecision(SARON_3_month_compounded.get_decimal_places())
		<< "For "
		<< avg_date
		<< " SARON 3 Month Compounded Average is "
		<< _3m_cmp->get_value()
		<< " and the same computed value is "
		<< SARON_average(SARON, rfd, avg_date, _3md).percent.get_value()
		<< endl;

	const auto& _6m_cmp = SARON_6_month_compounded[date];
	assert(_6m_cmp);

	cout
		<< fixed
		<< setprecision(SARON_6_month_compounded.get_decimal_places())
		<< "For "
		<< avg_date
		<< " SARON 6 Month Compounded Average is "
		<< _6m_cmp->get_value()
		<< " and the same computed value is "
		<< SARON_average(SARON, rfd, avg_date, _6md).percent.get_value()
		<< endl;

	const auto& _9m_cmp = SARON_9_month_compounded[date];
	assert(_9m_cmp);

	cout
		<< fixed
		<< setprecision(SARON_9_month_compounded.get_decimal_places())
		<< "For "
		<< avg_date
		<< " SARON 9 Month Compounded Average is "
		<< _9m_cmp->get_value()
		<< " and the same computed value is "
		<< SARON_average(SARON, rfd, avg_date, _9md).percent.get_value()
		<< endl;

	const auto& _12m_cmp = SARON_12_month_compounded[date];
	assert(_12m_cmp);

	cout
		<< fixed
		<< setprecision(SARON_12_month_compounded.get_decimal_places())
		<< "For "
		<< avg_date
		<< " SARON 12 Month Compounded Average is "
		<< _12m_cmp->get_value()
		<< " and the same computed value is "
		<< SARON_average(SARON, rfd, avg_date, _12md).percent.get_value()
		<< endl;

	return 0;
}
