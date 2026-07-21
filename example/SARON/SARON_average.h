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

#pragma once

#include <chrono>
#include <vector>
#include <utility>

#include <term.h>
#include <rate.h>
#include <fixings.h>
#include <average.h>
#include <reset_math.h>

#include <business_day_convention.h>
#include <following.h>
#include <modified_following.h>
#include <preceding.h>
#include <modified_preceding.h>
#include <day_count.h>

#include <calendar.h>
#include <calendar_algorithms.h>
#include <period.h>

#include "SARON_calendar_algorithms.h"



auto _SARON_average_start(
	const std::chrono::year_month_day& ymd,
	const reset::term& term,
	const gregorian::calendar& cal
) -> std::chrono::year_month_day;

auto _SARON_1_week_average_start(
	const std::chrono::year_month_day& ymd,
	const reset::term& term,
	const gregorian::calendar& cal
) -> std::chrono::year_month_day;

// from "Swiss Index. Rulebook Swiss Reference Rates."
[[nodiscard]] inline auto SARON_average(
	const reset::RateFixings& fix,
	const reset::rate_fixings_detail& rfd,
	const std::chrono::year_month_day& ymd,
	const reset::average_detail& detail = reset::average_detail{} // does it need a default?
) -> reset::rate
{
	// we effectively ignore average_detail.business_day_convention, which is not clean
	// should SARON's calculation of the start date be implemented as another business day convention?

	using namespace boost::decimal::literals;

	// do we handle the case where detail.term is empty?

	// implement in terms of compounded?
	// factor out more common code between SARON_average and average?

	const auto& cal = fix.get_calendar();

	const auto average_start = detail.term == reset::term{ std::chrono::weeks{ 1 } } ?
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

	rate = reset::round_dp(rate, detail.final_round);

	return {
		std::move(rate),
		reset::rate_detail{
			.start = average_start,
			.end = average_end,
			.day_count = rfd.day_count, // or should the average has its own day count? (is there a way to default it to underlying daily rate day count?)
			.round = detail.final_round
		}
	};
}

inline auto _SARON_average_start(
	const std::chrono::year_month_day& ymd,
	const reset::term& term,
	const gregorian::calendar& cal
) -> std::chrono::year_month_day // please note multiple return points
{
	if (is_last_business_day_of_month(ymd, cal))
	{
		const auto start_date = reset::retreat(ymd, term);
		return get_last_business_day_of_month(start_date.year() / start_date.month(), cal);
	}
	// If the end date falls on the last business day of a month, the start date must also be the last business day of a month.

	// Above is part of EoM convention, which we should factor out
	// (not sure if it should sit with business_day_convention or with term, as it is a combination of both)

	const auto date = fin_calendar::make_business_day(
		reset::retreat(ymd, term),
		fin_calendar::modified_preceding{},
		cal
	);

	const auto candidates = cal.make_business_days_schedule(
		gregorian::util::days_period{
			gregorian::shift_business_days(date, std::chrono::days{ -3 }, cal),
			date
		}
	); // -3/0 were chosen empirically

	auto starts = std::vector<std::chrono::year_month_day>{};
	for (const auto& can : candidates.get_dates())
	{
		const auto end_date_unadjusted = reset::advance(can, term);
		const auto end_date = is_last_business_day_of_month(can, cal) ?
			get_last_business_day_of_month(
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

inline auto _SARON_1_week_average_start(
	const std::chrono::year_month_day& ymd,
	const reset::term& term,
	const gregorian::calendar& cal
) -> std::chrono::year_month_day // please note multiple return points
{
	const auto date = fin_calendar::make_business_day(
		reset::retreat(ymd, term),
		fin_calendar::preceding{},
		cal
	);

	const auto candidates = cal.make_business_days_schedule(
		gregorian::util::days_period{
			gregorian::shift_business_days(date, std::chrono::days{ -3 }, cal),
			date
		}
	); // -3/0 were chosen empirically

	auto starts = std::vector<std::chrono::year_month_day>{};
	for (const auto& can : candidates.get_dates())
	{
		const auto end_date_unadjusted = reset::advance(can, term);
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
