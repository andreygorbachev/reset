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
#include <index.h>
#include <fixings.h>
#include <reset_math.h>

#include <actual_360.h>
#include <preceding.h>

#include <period.h>
#include <weekend.h>
#include <schedule.h>
#include <calendar.h>
#include <static_data.h>

#include <chrono>
#include <iostream>
#include <iomanip>
#include <ios>
#include <cassert>

using namespace std;
using namespace std::chrono;

using namespace gregorian;
using namespace gregorian::util;
using namespace gregorian::static_data;

using namespace fin_calendar;

using namespace reset;



// from https://www.banxico.org.mx/SieInternet/consultarDirectorioInternetAction.do?sector=18&accion=consultarCuadro&idCuadro=CF101&locale=en

static auto parse_csv_fixings_FTIIE() -> RateFixings
{
	return parse_csv_fixings<RateFixings>(
		"CF101.csv",
		1u,
		2006y / January / 2d,
		2026y / May / 5d,
		locate_calendar("America/CNBV", 2026y / May / 6d),
		2u
	);
}

static auto parse_csv_fixings_TIIE_fallback_28_day() -> RateFixings
{
	return parse_csv_fixings<RateFixings>(
		"CF101.csv",
		2u,
		2006y / January / 31d, // not 100% sure why these started later than the index
		2026y / May / 6d,
		locate_calendar("America/CNBV", 2026y / May / 6d),
		4u
	);
}

static auto parse_csv_fixings_TIIE_fallback_91_day() -> RateFixings
{
	return parse_csv_fixings<RateFixings>(
		"CF101.csv",
		3u,
		2008y / August / 4d, // not 100% sure why these started later than the index // why 28 and 91 started on different dates?
		2026y / May / 6d,
		locate_calendar("America/CNBV", 2026y / May / 6d),
		4u
	);
}

static auto parse_csv_fixings_TIIE_fallback_182_day() -> RateFixings
{
	return parse_csv_fixings<RateFixings>(
		"CF101.csv",
		4u,
		2024y / January / 3d, // not 100% sure why these started later than the index // why 28 and 182 started on different dates?
		2026y / May / 6d,
		locate_calendar("America/CNBV", 2026y / May / 6d),
		4u
	);
}

static auto make_empty_calendar()
{
	return calendar{
		NoWeekend,
		schedule{
			days_period{ 2006y / January / 2d, 2026y / May / 6d },
			{}
		}
	};
}

static auto parse_csv_fixings_target_rate() -> RateFixings
{
	return parse_csv_fixings<RateFixings>(
		"CF101.csv",
		0u,
		2025y / January / 1d, // 2008y / January / 21d,
		2026y / May / 6d,
		make_empty_calendar(), // locate_calendar("America/CNBV", 2026y / May / 6d),
		4u // 2u?
	);
}

static auto parse_csv_fixings_FTIIE_compounded_on_business_days_index() -> IndexFixings
{
	return parse_csv_fixings<IndexFixings>(
		"CF101.csv",
		6u,
		2006y / January / 2d,
		2026y / May / 6d,
		make_empty_calendar(),
		4u
	);
}

static auto parse_csv_fixings_FTIIE_compounded_on_calendar_days_index() -> IndexFixings
{
	return parse_csv_fixings<IndexFixings>(
		"CF101.csv",
		5u,
		2006y / January / 2d,
		2026y / May / 6d,
		make_empty_calendar(),
		4u
	);
}


static auto parse_csv_fixings_FTIIE_compounded_in_advance_28_day() -> RateFixings
{
	return parse_csv_fixings<RateFixings>(
		"CF101.csv",
		7u,
		2006y / January / 31d, // not 100% sure why these started later than the index
		2026y / May / 6d,
		locate_calendar("America/CNBV", 2026y / May / 6d),
		4u
	);
}

static auto parse_csv_fixings_FTIIE_compounded_in_advance_91_day() -> RateFixings
{
	return parse_csv_fixings<RateFixings>(
		"CF101.csv",
		8u,
		2006y / January / 31d,
		2026y / May / 6d,
		locate_calendar("America/CNBV", 2026y / May / 6d),
		4u
	);
}

static auto parse_csv_fixings_FTIIE_compounded_in_advance_182_day() -> RateFixings
{
	return parse_csv_fixings<RateFixings>(
		"CF101.csv",
		9u,
		2006y / January / 31d,
		2026y / May / 6d,
		locate_calendar("America/CNBV", 2026y / May / 6d),
		4u
	);
}



static auto non_business_day_index( // is this important enough to move to the main library?
	const RateFixings& fix,
	const rate_fixing_detail& rfd,
	const std::chrono::year_month_day& ymd,
	const index_detail& id = index_detail{} // does it need a default?
)
{
	if (fix.get_calendar().is_business_day(ymd))
		return index(fix, rfd, ymd, id);
	else
	{
		constexpr auto preceding = fin_calendar::preceding{};
		const auto prev = preceding.adjust(ymd, fix.get_calendar());

		auto indx = index(fix, rfd, prev, id).get_value();
		index_step_(indx, prev, ymd, fix, rfd, id);

		if (id.final_trunc)
			indx = trunc_dp(indx, *id.final_trunc);

		if (id.final_round)
			indx = round_dp(indx, *id.final_round);

		return Value{ indx };
	}
}

static auto compounded_in_advance( // is this important enough to move to the main library?
	const IndexFixings& fix,
	const std::chrono::year_month_day& d,
	const Decimal& tenor
)
{
	const auto& _index_d = fix[d];
	assert(_index_d); // we assume that requests are only made for business days, but actually index is given for all calendar days
	const auto index_d = static_cast<Decimal>(*_index_d);

	const auto d_28n = sys_days{ d } - days{ 28 };
	const auto& _index_d_28n = fix[d_28n];
	assert(_index_d_28n);
	const auto index_d_28n = static_cast<Decimal>(*_index_d_28n);

	const auto _1 = Decimal{ 1 };
	const auto _28 = Decimal{ 28 };
	const auto _360 = Decimal{ 360 };

	auto rate = Decimal{ (pow(index_d / index_d_28n, tenor / _28) - _1) * _360 / tenor }; // should we use day count?
	rate = round_dp(rate, 6u); // or should we be able to apply 4dp to the resulting percentage? (that would be closer to the documentation, which deals in percents)
	// should round_dp accept units for the power? (6dp or something like that)

	return Percent{ rate };
}

// or should we call it synthetic?
static auto fallback( // is this important enough to move to the main library?
	const RateFixings& fix,
	const RateFixings& target_rate_fix,
	const std::chrono::year_month_day& d, // do we assume it is always a good business day?
	const Decimal& tenor
)
{
	constexpr auto preceding = fin_calendar::preceding{};
	const auto prev = preceding.adjust(
		sys_days{ d } - days{ 1 }, 
		fix.get_calendar()
	);
	const auto prevprev = preceding.adjust(
		prev - days{ 1 }, // find source for 2 business day delay
		fix.get_calendar()
	);

	const auto& _fixing = fix[prevprev];
	assert(_fixing);
	const auto fixing = static_cast<Decimal>(*_fixing);

	const auto& _target_rate_prev_fixing = target_rate_fix[prev];
	assert(_target_rate_prev_fixing);
	const auto& _target_rate_prevprev_fixing = target_rate_fix[prevprev];
	assert(_target_rate_prevprev_fixing);
	const auto Banxico_move =
		static_cast<Decimal>(*_target_rate_prev_fixing) -
		static_cast<Decimal>(*_target_rate_prevprev_fixing);

	const auto _spread = BasisPoints{ "24" }; // constexpr? // is this right that it is the same spread for all tenors?
	const auto spread = static_cast<Decimal>(_spread);

	const auto _1 = Decimal{ 1 };
	const auto _360 = Decimal{ 360 };

	auto rate = Decimal{ (pow(_1 + (fixing + Banxico_move) / _360, tenor) - _1) * _360 / tenor }; // should we use day count?
	rate = round_dp(rate, 6u); // or should we be able to apply 4dp to the resulting percentage? (that would be closer to the documentation, which deals in percents)
	// should round_dp accept units for the power? (6dp or something like that)

	return Percent{ rate + spread }; // or do we need to apply the spread before rounding?
}



int main()
{
	const auto FTIIE = parse_csv_fixings_FTIIE();

	auto rfd = rate_fixing_detail{};
	rfd.day_count = actual_360<Decimal>{};

	const auto target_rate = parse_csv_fixings_target_rate();
	const auto TIIE_fallback_28_day = parse_csv_fixings_TIIE_fallback_28_day();
	const auto TIIE_fallback_91_day = parse_csv_fixings_TIIE_fallback_91_day();
	const auto TIIE_fallback_182_day = parse_csv_fixings_TIIE_fallback_182_day();

	const auto FTIIE_compounded_on_business_days_index = parse_csv_fixings_FTIIE_compounded_on_business_days_index();
	const auto FTIIE_compounded_on_calendar_days_index = parse_csv_fixings_FTIIE_compounded_on_calendar_days_index();

	const auto FTIIE_compounded_in_advance_28_day = parse_csv_fixings_FTIIE_compounded_in_advance_28_day();
	const auto FTIIE_compounded_in_advance_91_day = parse_csv_fixings_FTIIE_compounded_in_advance_91_day();
	const auto FTIIE_compounded_in_advance_182_day = parse_csv_fixings_FTIIE_compounded_in_advance_182_day();

	// from
	// "Determination of the Overnight Funding TIIE Index compounded on business days,
	// the Overnight Funding TIIE Index compounded on calendar days,
	// and the Compounded in advance Overnight Funding TIIE."
	auto bus_id = index_detail{};
	bus_id.initial_value = Value{ "100000" };
	bus_id.initial_date = 2006y / January / 2d;
	bus_id.step_round = 16u;
	bus_id.final_round = 4u;

	auto cal_id = index_detail{};
	cal_id.initial_value = Value{ "100000" };
	cal_id.initial_date = 2006y / January / 2d;
	cal_id.step_round = 16u;
	cal_id.final_round = 4u;
	cal_id.calendar = FTIIE_compounded_on_calendar_days_index.get_calendar();

	const auto date = 2026y / May / 6d;

	const auto& bus_indx = FTIIE_compounded_on_business_days_index[date];
	assert(bus_indx);
	assert(FTIIE.get_calendar().is_business_day(date));

	cout
		<< fixed
		<< setprecision(FTIIE_compounded_on_business_days_index.get_decimal_places())
		<< "For "
		<< date
		<< " F-TIIE Compounded Index (business days) is "
		<< bus_indx->get_value()
		<< " and the same computed value is "
		<< index(FTIIE, rfd, date, bus_id).get_value() // we checked that date is a business day
		<< endl;

	const auto& cal_indx = FTIIE_compounded_on_calendar_days_index[date];
	assert(cal_indx);

	cout
		<< fixed
		<< setprecision(FTIIE_compounded_on_calendar_days_index.get_decimal_places())
		<< "For "
		<< date
		<< " F-TIIE Compounded Index (calendar days) is "
		<< cal_indx->get_value()
		<< " and the same computed value is "
		<< index(FTIIE, rfd, date, cal_id).get_value()
		<< endl;

	const auto& _28d_indx = FTIIE_compounded_in_advance_28_day[date];
	assert(_28d_indx);

	cout
		<< fixed
		<< setprecision(FTIIE_compounded_in_advance_28_day.get_decimal_places())
		<< "For "
		<< date
		<< " F-TIIE Compounded In Advance Index (28 days) is "
		<< _28d_indx->get_value()
		<< " and the same computed value is "
		<< compounded_in_advance(FTIIE_compounded_on_business_days_index, date, Decimal{ 28 }).get_value()
		<< endl;

	const auto& _91d_indx = FTIIE_compounded_in_advance_91_day[date];
	assert(_28d_indx);

	cout
		<< fixed
		<< setprecision(FTIIE_compounded_in_advance_91_day.get_decimal_places())
		<< "For "
		<< date
		<< " F-TIIE Compounded In Advance Index (91 days) is "
		<< _91d_indx->get_value()
		<< " and the same computed value is "
		<< compounded_in_advance(FTIIE_compounded_on_business_days_index, date, Decimal{ 91 }).get_value()
		<< endl;

	const auto& _182d_indx = FTIIE_compounded_in_advance_182_day[date];
	assert(_182d_indx);

	cout
		<< fixed
		<< setprecision(FTIIE_compounded_in_advance_182_day.get_decimal_places())
		<< "For "
		<< date
		<< " F-TIIE Compounded In Advance Index (182 days) is "
		<< _182d_indx->get_value()
		<< " and the same computed value is "
		<< compounded_in_advance(FTIIE_compounded_on_business_days_index, date, Decimal{ 182 }).get_value()
		<< endl;

	const auto& _28d_fallback = TIIE_fallback_28_day[date];
	assert(_28d_fallback);

	cout
		<< fixed
		<< setprecision(TIIE_fallback_28_day.get_decimal_places())
		<< "For "
		<< date
		<< " TIIE Fallback (28 days) is "
		<< _28d_fallback->get_value()
		<< " and the same computed value is "
		<< fallback(FTIIE, target_rate, date, Decimal{ 28 }).get_value()
		<< endl;

	const auto& _91d_fallback = TIIE_fallback_91_day[date];
	assert(_91d_fallback);

	cout
		<< fixed
		<< setprecision(TIIE_fallback_91_day.get_decimal_places())
		<< "For "
		<< date
		<< " TIIE Fallback (91 days) is "
		<< _91d_fallback->get_value()
		<< " and the same computed value is "
		<< fallback(FTIIE, target_rate, date, Decimal{ 91 }).get_value()
		<< endl;

	const auto& _182d_fallback = TIIE_fallback_182_day[date];
	assert(_182d_fallback);

	cout
		<< fixed
		<< setprecision(TIIE_fallback_182_day.get_decimal_places())
		<< "For "
		<< date
		<< " TIIE Fallback (182 days) is "
		<< _182d_fallback->get_value()
		<< " and the same computed value is "
		<< fallback(FTIIE, target_rate, date, Decimal{ 182 }).get_value()
		<< endl;

	// look for inconsistencies in the index data
/*
	const auto bus_period = FTIIE_compounded_on_business_days_index.get_time_series().get_period();
	for (
		auto d = bus_period.get_from();
		d <= bus_period.get_until();
		d = sys_days{ d } + days{ 1 }
	)
	{
		const auto& fix = FTIIE_compounded_on_business_days_index[d];
		assert(fix); // index is published for each calendar day
		const auto computed_fix = non_business_day_index(FTIIE, rfd, d, bus_id); // also handles business days
		if (*fix != computed_fix)
			cout
				<< "For "
				<< d
				<< " F-TIIE Compounded Index (business days) is "
				<< fix->get_value()
				<< " and the same computed value is "
				<< computed_fix.get_value()
				<< endl;
	}

	const auto cal_period = FTIIE_compounded_on_calendar_days_index.get_time_series().get_period();
	for (
		auto d = cal_period.get_from();
		d <= cal_period.get_until();
		d = sys_days{ d } + days{ 1 }
	)
	{
		const auto& fix = FTIIE_compounded_on_calendar_days_index[d];
		assert(fix); // index is published for each calendar day
		const auto computed_fix = index(FTIIE, rfd, d, cal_id);
		if (*fix != computed_fix)
			cout
				<< "For "
				<< d
				<< " F-TIIE Compounded Index (calendar days) is "
				<< fix->get_value()
				<< " and the same computed value is "
				<< computed_fix.get_value()
				<< endl;
	}
*/
	const auto& _28d_fallback_calendar = TIIE_fallback_28_day.get_calendar();
	const auto _28d_fallback_dates = _28d_fallback_calendar.make_business_days_schedule(
//		TIIE_fallback_28_day.get_time_series().get_period()
		days_period{ 2025y / January / 6d, date }
	);
	for (const auto& d : _28d_fallback_dates.get_dates())
	{
		const auto& fix = TIIE_fallback_28_day[d];
		assert(fix);
		const auto fb = fallback(FTIIE, target_rate, d, Decimal{ 28 });
		if (*fix != fb)
			cout
				<< fixed
				<< setprecision(TIIE_fallback_28_day.get_decimal_places())
				<< "For "
				<< d
				<< " TIIE Fallback (28 days) is "
				<< fix->get_value()
				<< " and the same computed value is "
				<< fb.get_value()
				<< endl;
	}

	const auto& _91d_fallback_calendar = TIIE_fallback_91_day.get_calendar();
	const auto _91d_fallback_dates = _91d_fallback_calendar.make_business_days_schedule(
//		TIIE_fallback_91_day.get_time_series().get_period()
		days_period{ 2025y / January / 6d, date }
	);
	for (const auto& d : _91d_fallback_dates.get_dates())
	{
		const auto& fix = TIIE_fallback_91_day[d];
		assert(fix);
		const auto fb = fallback(FTIIE, target_rate, d, Decimal{ 91 });
		if (*fix != fb)
			cout
			<< fixed
			<< setprecision(TIIE_fallback_91_day.get_decimal_places())
			<< "For "
			<< d
			<< " TIIE Fallback (91 days) is "
			<< fix->get_value()
			<< " and the same computed value is "
			<< fb.get_value()
			<< endl;
	}

	const auto& _182d_fallback_calendar = TIIE_fallback_182_day.get_calendar();
	const auto _182d_fallback_dates = _182d_fallback_calendar.make_business_days_schedule(
//		TIIE_fallback_182_day.get_time_series().get_period()
		days_period{ 2025y / January / 6d, date }
	);
	for (const auto& d : _182d_fallback_dates.get_dates())
	{
		const auto& fix = TIIE_fallback_182_day[d];
		assert(fix);
		const auto fb = fallback(FTIIE, target_rate, d, Decimal{ 182 });
		if (*fix != fb)
			cout
			<< fixed
			<< setprecision(TIIE_fallback_182_day.get_decimal_places())
			<< "For "
			<< d
			<< " TIIE Fallback (182 days) is "
			<< fix->get_value()
			<< " and the same computed value is "
			<< fb.get_value()
			<< endl;
	}

	return 0;
}
