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
#include <term.h>
#include <reset_math.h>
#include <calendar_algorithms.h>

#include <day_count.h>
#include <actual_360.h>
#include <business_day_convention.h>
#include <preceding.h>
#include <modified_preceding.h>
#include <following.h>
#include <modified_following.h>

#include <period.h>
#include <schedule.h>
#include <calendar.h>

#include <boost/decimal.hpp>

#include <chrono>
#include <iostream>
#include <iomanip>
#include <ios>
#include <cassert>
#include <optional>
#include <vector>
#include <utility>
#include <ranges>
#include <future>
#include <syncstream>

using namespace std;
using namespace std::chrono;

using namespace boost::decimal;
using namespace boost::decimal::literals;

using namespace gregorian;

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



// should these 2 functions be in calendar? (or if they are more finance specific in fin-calendar/util?)
static auto _get_last_business_day_of_month(
	const std::chrono::year_month& ym,
	const gregorian::calendar& cal
) -> std::chrono::year_month_day
{
	constexpr auto preceding = fin_calendar::preceding{};
	const auto candidate = std::chrono::year_month_day{ ym / last };
	return preceding.adjust(candidate, cal);
}

static auto _is_last_business_day_of_month(
	const std::chrono::year_month_day& ymd,
	const gregorian::calendar& cal
) -> bool
{
	return ymd == _get_last_business_day_of_month(ymd.year() / ymd.month(), cal);
}

static auto _SARON_average_start(
	const std::chrono::year_month_day& ymd,
	const term& term,
	const gregorian::calendar& cal
) -> std::chrono::year_month_day // please note multiple return points
{
	if (_is_last_business_day_of_month(ymd, cal))
	{
		const auto start_date = retreat(ymd, term);
		return _get_last_business_day_of_month(start_date.year() / start_date.month(), cal);
	}
	// If the end date falls on the last business day of a month, the start date must also be the last business day of a month.

	// Above is part of EoM convention, which we should factor out
	// (not sure if it should sit with business_day_convention or with term, as it is a combination of both)

	const auto date = fin_calendar::make_business_day(
		retreat(ymd, term),
		fin_calendar::modified_preceding{},
		cal
	);

	const auto candidates = cal.make_business_days_schedule(
		gregorian::util::days_period{
			gregorian::shift_business_days(date, days{ -3 }, cal),
			date
		}
	); // -3/0 were chosen empirically

	auto starts = std::vector<std::chrono::year_month_day>{};
	for (const auto& can : candidates.get_dates())
	{
		const auto end_date_unadjusted = advance(can, term);
		const auto end_date = _is_last_business_day_of_month(can, cal) ?
			_get_last_business_day_of_month(
				end_date_unadjusted.year() / end_date_unadjusted.month(),
				cal
			) :
			fin_calendar::make_business_day(
				end_date_unadjusted,
				fin_calendar::modified_following{},
				cal
			);

		if (end_date == ymd)
			starts.push_back(can);
	}

	if (starts.empty())
		return date;
	// If the originally calculated start date falls on a non-business day or non-existent date (e.g. 30th of February),
	// the business day preceding the calculated start date will be used as the start date,
	// unless this new start date would fall in a different month.
	// In this case, the following business day will be used as the start date and not the previous business day.

	const auto mid_index = (starts.size() - 1) / 2;
	// If the date is unique according to the CHF money market calendar, it will be used as the start date.
	// For each end date with several possible start dates according to the CHF money market calendar,
	// the following applies(unless the end date is the last business day of a month):
	//	In case of an uneven number of possible start dates, the middle date will be chosen as the start date
	//	In case of an even number of possible start dates, the earlier of the two middle dates will be chosen

	return starts[mid_index];
}

static auto _SARON_1_week_average_start(
	const std::chrono::year_month_day& ymd,
	const term& term,
	const gregorian::calendar& cal
) -> std::chrono::year_month_day // please note multiple return points
{
	const auto date = fin_calendar::make_business_day(
		retreat(ymd, term),
		fin_calendar::preceding{},
		cal
	);

	const auto candidates = cal.make_business_days_schedule(
		gregorian::util::days_period{
			gregorian::shift_business_days(date, days{ -3 }, cal),
			date
		}
	); // -3/0 were chosen empirically

	auto starts = std::vector<std::chrono::year_month_day>{};
	for (const auto& can : candidates.get_dates())
	{
		const auto end_date_unadjusted = advance(can, term);
		const auto end_date = fin_calendar::make_business_day(end_date_unadjusted, fin_calendar::following{}, cal);

		if (end_date == ymd)
			starts.push_back(can);
	}

	if (starts.empty())
		return date;
	// If the originally calculated start date falls on a non-business day or non-existent date (e.g. 30th of February),
	// the business day preceding the calculated start date will be used as the start date

	const auto mid_index = (starts.size() - 1) / 2;
	// If the date is unique according to the CHF money market calendar, it will be used as the start date.
	// For each end date with several possible start dates according to the CHF money market calendar,
	// the following applies(unless the end date is the last business day of a month):
	//	In case of an uneven number of possible start dates, the middle date will be chosen as the start date
	//	In case of an even number of possible start dates, the earlier of the two middle dates will be chosen

	return starts[mid_index];
}

// from "Swiss Index. Rulebook Swiss Reference Rates."
static auto SARON_average(
	const RateFixings& fix,
	const rate_fixings_detail& rfd,
	const std::chrono::year_month_day& ymd,
	const average_detail& detail = average_detail{} // does it need a default?
) -> rate
{
	// we effectively ignore average_detail.business_day_convention, which is not clean
	// should SARON's calculation of the start date be implemented as another business day convention?

	using namespace boost::decimal::literals;

	// do we handle the case where detail.term is empty?

	// implement in terms of compounded?
	// factor out more common code between SARON_average and average?

	const auto& cal = fix.get_calendar();

	const auto average_start = detail.term == term{ std::chrono::weeks{ 1 } } ?
		_SARON_1_week_average_start(ymd, detail.term, cal) :
		_SARON_average_start(ymd, detail.term, cal);
	// In general, SARON Compound Rates with a tenor of less than one month (e.g. weekly) simplify the determination of the start- and end dates,
	// since the month-end restrictions are omitted. However, the other conventions of the money market calendar must be maintained.
	//
	// (at the moment we only assume 1w)

	const auto average_end = ymd; // I think we assume that ymd is a good business day - should we check for that?

	const auto schedule = cal.make_business_days_schedule(
		gregorian::util::days_period{ average_start, average_end }
	); // is this a wrong data structure?
	// assert that it is not empty?

	auto dates = schedule.get_dates(); // we might consider something not making a copy as most use cases would not need to insert
	if (!dates.contains(average_start)) // or we can just have a look at cbegin(), which is O(1) operation on most platforms, rather than O(log n)
		dates.insert(average_start); // do it with hint?

	auto val = 1_dl;

	for (const auto& [start, end] : dates | std::views::adjacent<2uz>)
		average_step_(val, start, end, fix, rfd);

	const auto year_fraction = fin_calendar::fraction(schedule.get_period(), rfd.day_count);

	auto rate = (val - 1_dl) / year_fraction;

	rate = round_dp(rate, detail.final_round);

	return {
		std::move(rate),
		rate_detail{
			.start = average_start,
			.end = average_end,
			.day_count = rfd.day_count, // or should the average has its own day count? (is there a way to default it to underlying daily rate day count?)
			.round = detail.final_round
		}
	};
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
		.business_day_convention = {}, // hardcoded in SARON_average_start
		.final_round = 4u + 2u // as we deal with fractions, rather than rates
	};

	constexpr auto _1md = average_detail{
		.term = months{ 1 },
		.business_day_convention = {},
		.final_round = 4u + 2u // as we deal with fractions, rather than rates
	};

	constexpr auto _2md = average_detail{
		.term = months{ 2 },
		.business_day_convention = {},
		.final_round = 4u + 2u // as we deal with fractions, rather than rates
	};

	constexpr auto _3md = average_detail{
		.term = months{ 3 },
		.business_day_convention = {},
		.final_round = 4u + 2u // as we deal with fractions, rather than rates
	};

	constexpr auto _6md = average_detail{
		.term = months{ 6 },
		.business_day_convention = {},
		.final_round = 4u + 2u // as we deal with fractions, rather than rates
	};

	constexpr auto _9md = average_detail{
		.term = months{ 9 },
		.business_day_convention = {},
		.final_round = 4u + 2u // as we deal with fractions, rather than rates
	};

	constexpr auto _12md = average_detail{
		.term = months{ 12 },
		.business_day_convention = {},
		.final_round = 4u + 2u // as we deal with fractions, rather than rates
	};

	const auto& SARON_calendar = SARON.get_calendar();

	// test _SARON_average_start
	assert(_SARON_average_start(2018y / April / 30d, months{ 1 }, SARON_calendar) == 2018y / March / 29d);
	assert(_SARON_average_start(2018y / June / 15d, months{ 1 }, SARON_calendar) == 2018y / May / 15d);
	assert(_SARON_average_start(2018y / October / 8d, months{ 1 }, SARON_calendar) == 2018y / September / 6d);
	assert(_SARON_average_start(2018y / April / 23d, months{ 1 }, SARON_calendar) == 2018y / March / 22d);
	assert(_SARON_average_start(2019y / December / 10d, months{ 1 }, SARON_calendar) == 2019y / November / 8d);

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

	const auto avg_date = shift_business_days(date, days{ 1 }, SARON_calendar); // need to think about what date is used in the files - should we use end_date?

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

	// look for inconsistencies in the data

	auto SARON_1_week_compounded_task = async(launch::async, [&]() {
		auto scout = osyncstream{ cout };

		const auto& SARON_1_week_compounded_calendar = SARON_1_week_compounded.get_calendar();
		const auto _1_week_dates = SARON_1_week_compounded_calendar.make_business_days_schedule(
			SARON_1_week_compounded.get_time_series().get_period()
		);
		for (const auto& d : _1_week_dates.get_dates())
		{
			const auto& _1w_avg = SARON_1_week_compounded[d];
			assert(_1w_avg);

			const auto avg_date = shift_business_days(d, days{ 1 }, SARON_calendar);

			if (*_1w_avg != SARON_average(SARON, rfd, avg_date, _1wd).percent)
				scout
					<< fixed
					<< setprecision(SARON_1_week_compounded.get_decimal_places())
					<< "For "
					<< avg_date
					<< " SARON 1 Week Compounded Average is "
					<< SARON_1_week_compounded[d]->get_value()
					<< " and the same computed value is "
					<< SARON_average(SARON, rfd, avg_date, _1wd).percent.get_value()
					<< endl;
		}
	});

	auto SARON_1_month_compounded_task = async(launch::async, [&]() {
		auto scout = osyncstream{ cout };

		const auto& SARON_1_month_compounded_calendar = SARON_1_month_compounded.get_calendar();
		const auto _1_month_dates = SARON_1_month_compounded_calendar.make_business_days_schedule(
			SARON_1_month_compounded.get_time_series().get_period()
		);
		for (const auto& d : _1_month_dates.get_dates())
		{
			const auto& _1m_avg = SARON_1_month_compounded[d];
			assert(_1m_avg);

			const auto avg_date = shift_business_days(d, days{ 1 }, SARON_calendar);

			if (*_1m_avg != SARON_average(SARON, rfd, avg_date, _1md).percent)
				scout
					<< fixed
					<< setprecision(SARON_1_month_compounded.get_decimal_places())
					<< "For "
					<< avg_date
					<< " SARON 1 Month Compounded Average is "
					<< SARON_1_month_compounded[d]->get_value()
					<< " and the same computed value is "
					<< SARON_average(SARON, rfd, avg_date, _1md).percent.get_value()
					<< endl;
		}
	});

	auto SARON_2_month_compounded_task = async(launch::async, [&]() {
		auto scout = osyncstream{ cout };

		const auto& SARON_2_month_compounded_calendar = SARON_2_month_compounded.get_calendar();
		const auto _2_month_dates = SARON_2_month_compounded_calendar.make_business_days_schedule(
			SARON_2_month_compounded.get_time_series().get_period()
		);
		for (const auto& d : _2_month_dates.get_dates())
		{
			const auto& _2m_avg = SARON_2_month_compounded[d];
			assert(_2m_avg);

			const auto avg_date = shift_business_days(d, days{ 1 }, SARON_calendar);

			if (*_2m_avg != SARON_average(SARON, rfd, avg_date, _2md).percent)
				scout
					<< fixed
					<< setprecision(SARON_2_month_compounded.get_decimal_places())
					<< "For "
					<< avg_date
					<< " SARON 2 Month Compounded Average is "
					<< SARON_2_month_compounded[d]->get_value()
					<< " and the same computed value is "
					<< SARON_average(SARON, rfd, avg_date, _2md).percent.get_value()
					<< endl;
		}
	});

	auto SARON_3_month_compounded_task = async(launch::async, [&]() {
		auto scout = osyncstream{ cout };

		const auto& SARON_3_month_compounded_calendar = SARON_3_month_compounded.get_calendar();
		const auto _3_month_dates = SARON_3_month_compounded_calendar.make_business_days_schedule(
			SARON_3_month_compounded.get_time_series().get_period()
		);
		for (const auto& d : _3_month_dates.get_dates())
		{
			const auto& _3m_avg = SARON_3_month_compounded[d];
			assert(_3m_avg);

			const auto avg_date = shift_business_days(d, days{ 1 }, SARON_calendar);

			if (*_3m_avg != SARON_average(SARON, rfd, avg_date, _3md).percent)
				scout
					<< fixed
					<< setprecision(SARON_3_month_compounded.get_decimal_places())
					<< "For "
					<< avg_date
					<< " SARON 3 Month Compounded Average is "
					<< SARON_3_month_compounded[d]->get_value()
					<< " and the same computed value is "
					<< SARON_average(SARON, rfd, avg_date, _3md).percent.get_value()
					<< endl;
		}
	});

	auto SARON_6_month_compounded_task = async(launch::async, [&]() {
		auto scout = osyncstream{ cout };

		const auto& SARON_6_month_compounded_calendar = SARON_6_month_compounded.get_calendar();
		const auto _6_month_dates = SARON_6_month_compounded_calendar.make_business_days_schedule(
			SARON_6_month_compounded.get_time_series().get_period()
		);
		for (const auto& d : _6_month_dates.get_dates())
		{
			const auto& _6m_avg = SARON_6_month_compounded[d];
			assert(_6m_avg);

			const auto avg_date = shift_business_days(d, days{ 1 }, SARON_calendar);

			if (*_6m_avg != SARON_average(SARON, rfd, avg_date, _6md).percent)
				scout
					<< fixed
					<< setprecision(SARON_6_month_compounded.get_decimal_places())
					<< "For "
					<< avg_date
					<< " SARON 6 Month Compounded Average is "
					<< SARON_6_month_compounded[d]->get_value()
					<< " and the same computed value is "
					<< SARON_average(SARON, rfd, avg_date, _6md).percent.get_value()
					<< endl;
		}
	});

	auto SARON_9_month_compounded_task = async(launch::async, [&]() {
		auto scout = osyncstream{ cout };

		const auto& SARON_9_month_compounded_calendar = SARON_9_month_compounded.get_calendar();
		const auto _9_month_dates = SARON_9_month_compounded_calendar.make_business_days_schedule(
			SARON_9_month_compounded.get_time_series().get_period()
		);
		for (const auto& d : _9_month_dates.get_dates())
		{
			const auto& _9m_avg = SARON_9_month_compounded[d];
			assert(_9m_avg);

			const auto avg_date = shift_business_days(d, days{ 1 }, SARON_calendar);

			if (*_9m_avg != SARON_average(SARON, rfd, avg_date, _9md).percent)
				scout
					<< fixed
					<< setprecision(SARON_9_month_compounded.get_decimal_places())
					<< "For "
					<< avg_date
					<< " SARON 9 Month Compounded Average is "
					<< SARON_9_month_compounded[d]->get_value()
					<< " and the same computed value is "
					<< SARON_average(SARON, rfd, avg_date, _9md).percent.get_value()
					<< endl;
		}
	});

	auto SARON_12_month_compounded_task = async(launch::async, [&]() {
		auto scout = osyncstream{ cout };

		const auto& SARON_12_month_compounded_calendar = SARON_12_month_compounded.get_calendar();
		const auto _12_month_dates = SARON_12_month_compounded_calendar.make_business_days_schedule(
			SARON_12_month_compounded.get_time_series().get_period()
		);
		for (const auto& d : _12_month_dates.get_dates())
		{
			const auto& _12m_avg = SARON_12_month_compounded[d];
			assert(_12m_avg);

			const auto avg_date = shift_business_days(d, days{ 1 }, SARON_calendar);

			if (*_12m_avg != SARON_average(SARON, rfd, avg_date, _12md).percent)
				scout
					<< fixed
					<< setprecision(SARON_12_month_compounded.get_decimal_places())
					<< "For "
					<< avg_date
					<< " SARON 12 Month Compounded Average is "
					<< SARON_12_month_compounded[d]->get_value()
					<< " and the same computed value is "
					<< SARON_average(SARON, rfd, avg_date, _12md).percent.get_value()
					<< endl;
		}
	});

	SARON_1_week_compounded_task.get();
	SARON_1_month_compounded_task.get();
	SARON_2_month_compounded_task.get();
	SARON_3_month_compounded_task.get();
	SARON_6_month_compounded_task.get();
	SARON_9_month_compounded_task.get();
	SARON_12_month_compounded_task.get();

	return 0;
}
