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

#include <decimal.h>
#include <fixings.h>
#include <index.h>
#include <scaled_value.h>

#include <actual_365_fixed.h>

#include <calendar.h>
#include <period.h>
#include <weekend.h>
#include <schedule.h>
#include <time_series.h>

#include <utility>
#include <optional>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <ios>

using namespace std;
using namespace std::chrono;

using namespace reset;

using namespace fin_calendar;

using namespace gregorian;
using namespace gregorian::util;



static auto make_fixings()
{
	const/*expr*/ auto daily_rate = Percent{ "3.17" }; // we assume that all rates are always the same

	auto ts = time_series<optional<Percent>>{ days_period{ 2025y / FirstDayOfJanuary, 2525y / LastDayOfDecember } };

	auto cal = calendar{ SaturdaySundayWeekend, schedule{ ts.get_period(), {} } }; // we ignore bank holidays

	const auto [f, u] = cal.get_schedule().get_period().from_until();
	for (
		auto d = f;
		d <= u;
		d = sys_days{ d } + days{ 1 }
	)
		if (cal.is_business_day(d))
			ts[d] = daily_rate;

	return fixings{ move(ts), move(cal), 8u };
}


int main()
{
	const auto fix = make_fixings();
	const auto& cal = fix.get_calendar();
	const auto [f, u] = cal.get_schedule().get_period().from_until();

	auto rfd = rate_fixing_detail{};
	rfd.day_count = actual_365_fixed<Decimal>{};

	auto id1 = index_detail{};
	id1.initial_value = Decimal{ 100 };
	id1.initial_date = f;
	id1.step_round = 18u; // we compare the index with daily rounding
	id1.final_round = 8u;

	auto id2 = id1;
	id2.step_round = nullopt; // with the index without daily rounding

	cout
		<< fixed
		<< setprecision(fix.get_decimal_places());
		
	for (
		auto d = f;
		d <= u;
		d = sys_days{ d } + days{ 365 }
	) // as we do not cache the previous step, we can spread the below over multiple threads
	{
		const auto i1 = index(fix, rfd, d, id1);
		const auto i2 = index(fix, rfd, d, id2);

		if (i1 != i2)
			cout
				<< "For "
				<< d
				<< " Index with step_round is "
				<< i1.get_value()
				<< " and Index without step_round is "
				<< i2.get_value()
				<< endl;
	}

	return 0;
}
