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

#include <actual_360.h>

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



static auto parse_csv_fixings_FTIIE() -> RateFixings
{
	// from https://www.banxico.org.mx/SieInternet/consultarDirectorioInternetAction.do?&sector=18&accion=consultarDirectorioCuadros&locale=en
	return parse_csv_fixings<RateFixings>(
		"Overnight TIIE Funding Rate.csv",
		0u,
		2006y / January / 2d,
		2026y / April / 24d,
		locate_calendar("America/CNBV", 2026y / April / 24u),
		2u
	);
}

static auto make_empty_calendar()
{
	return calendar{
		NoWeekend,
		schedule{
			days_period{ 2006y / January / 2d, 2026y / April / 27d },
			{}
		}
	};
}

static auto parse_csv_fixings_FTIIE_compounded_on_business_days_index() -> IndexFixings
{
	// from https://www.banxico.org.mx/SieInternet/consultarDirectorioInternetAction.do?sector=18&accion=consultarCuadroAnalitico&idCuadro=CA766&locale=en
	return parse_csv_fixings<IndexFixings>(
		"Overnight Funding TIIE indexes and compounded in advance Overnight Funding TIIE.csv",
		1u,
		2006y / January / 2d,
		2026y / April / 27d,
		make_empty_calendar(),
		4u
	);
}

static auto parse_csv_fixings_FTIIE_compounded_on_calendar_days_index() -> IndexFixings
{
	// from https://www.banxico.org.mx/SieInternet/consultarDirectorioInternetAction.do?sector=18&accion=consultarCuadroAnalitico&idCuadro=CA766&locale=en
	return parse_csv_fixings<IndexFixings>(
		"Overnight Funding TIIE indexes and compounded in advance Overnight Funding TIIE.csv",
		0u,
		2006y / January / 2d,
		2026y / April / 27d,
		make_empty_calendar(),
		4u
	);
}


static auto parse_csv_fixings_FTIIE_compounded_in_advance_28_day() -> RateFixings
{
	return parse_csv_fixings<RateFixings>(
		"Overnight Funding TIIE indexes and compounded in advance Overnight Funding TIIE.csv",
		2u,
		2006y / January / 31d, // not 100% sure why these started later than the index
		2026y / April / 27d,
		locate_calendar("America/CNBV", 2026y / April / 24u),
		4u
	);
}

static auto parse_csv_fixings_FTIIE_compounded_in_advance_91_day() -> RateFixings
{
	return parse_csv_fixings<RateFixings>(
		"Overnight Funding TIIE indexes and compounded in advance Overnight Funding TIIE.csv",
		3u,
		2006y / January / 31d,
		2026y / April / 27d,
		locate_calendar("America/CNBV", 2026y / April / 24u),
		4u
	);
}

static auto parse_csv_fixings_FTIIE_compounded_in_advance_182_day() -> RateFixings
{
	return parse_csv_fixings<RateFixings>(
		"Overnight Funding TIIE indexes and compounded in advance Overnight Funding TIIE.csv",
		4u,
		2006y / January / 31d,
		2026y / April / 27d,
		locate_calendar("America/CNBV", 2026y / April / 24u),
		4u
	);
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
	const auto index_d_28n = static_cast<Decimal>(*_index_d_28n);

	const auto _1 = Decimal{ 1 };
	const auto _28 = Decimal{ 28 };
	const auto _360 = Decimal{ 360 };

	auto rate = Decimal{ (pow(index_d / index_d_28n, tenor / _28) - _1) * _360 / tenor }; // should we use day count?
	rate = round_dp(rate, 6u); // or should we be able to apply 4dp to the resultin percentage? (that would be closer to the documentatio, which deals in percents)
	// should round_dp accept units for the power? (6dp or something like that)

	return Percent{ rate };
}



int main()
{
	const auto FTIIE = parse_csv_fixings_FTIIE();

	auto rfd = rate_fixing_detail{};
	rfd.day_count = actual_360<Decimal>{};

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

//	const auto date = 2026y / April / 27d;
	const auto date = 2026y / April / 24d;

	const auto& bus_indx = FTIIE_compounded_on_business_days_index[date];
	assert(bus_indx);

	cout
		<< fixed
		<< setprecision(FTIIE_compounded_on_business_days_index.get_decimal_places())
		<< "For "
		<< date
		<< " F-TIIE Compounded Index (business days) is "
		<< bus_indx->get_value()
		<< " and the same computed value is "
		<< index(FTIIE, rfd, date, bus_id).get_value()
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

	return 0;
}
