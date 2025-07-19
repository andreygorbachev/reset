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

#include <resets.h>
#include <index.h>

#include <day_count.h>

#include <calendar.h>
#include <period.h>
#include <schedule.h>
#include <time_series.h>

#include <utility>
#include <optional>
#include <chrono>
#include <iostream>
#include <iomanip>

using namespace std;
using namespace std::chrono;

using namespace boost::multiprecision;

using namespace reset;

using namespace fin_calendar;

using namespace gregorian;
using namespace gregorian::util;



auto make_resets()
{
	const/*expr*/ auto daily_rate = cpp_dec_float_50{ "7.33" }; // we assume that all rates are always the same

	auto ts = time_series<optional<cpp_dec_float_50>>{ days_period{ 2025y / FirstDayOfJanuary, 2125y / LastDayOfDecember } };

	auto cal = calendar{ SaturdaySundayWeekend, schedule{ ts.get_period(), {} } }; // we ignore bank holidays

	const auto [f, u] = cal.get_schedule().get_period().from_until();
	for (
		auto d = f;
		d <= u;
		d = sys_days{ d } + days{ 1 }
	)
	{
		if (cal.is_business_day(d))
		{
			ts[d] = daily_rate;
		}
	}

	return resets{ move(ts), move(cal), actual_365_fixed<cpp_dec_float_50>{} };
}


int main()
{
	const auto r = make_resets();
	const auto& cal = r.get_calendar();
	const auto [f, u] = cal.get_schedule().get_period().from_until();

	auto detail1 = index_detail{};
	detail1.initial_value = cpp_dec_float_50{ 100 };
	detail1.initial_date = f;
	detail1.step_round = 18u; // we compate the index with daily rounding
	detail1.final_round = 8u;

	auto detail2 = detail1;
	detail2.step_round = nullopt; // with the index without daily rounding

	for (
		auto d = f;
		d <= u;
		d = sys_days{ d } + days{ 100 }
	) // as we do not cache the previous step, we can spread below over multiple threads
	{
		cout
			<< fixed
			<< setprecision(8)
			<< "For "
			<< d
			<< " Index with step_round is "
			<< index(r, d, detail1)
			<< " and Index without step_round is "
			<< index(r, d, detail2)
			<< endl;
	}

	return 0;
}
