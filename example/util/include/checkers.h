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
#include <string>
#include <future>
#include <syncstream>
#include <ios>
#include <iomanip>
#include <iostream>
#include <cassert>

#include <fixings.h>
#include <index.h>
#include <average.h>

#include <calendar_algorithms.h>



[[nodiscard]] inline void make_compounded_index_check_task(
	const reset::RateFixings& rfr,
	const reset::rate_fixings_detail& rfr_detail,
	const reset::IndexFixings& indx,
	const reset::index_detail& indx_detail,
	const std::string& indx_label
)
{
	const auto period = indx.get_time_series().get_period();
	for (
		auto d = period.get_from();
		d <= period.get_until();
		d = std::chrono::sys_days{ d } + std::chrono::days{ 1 }
	)
	{
		const auto& fix = indx[d];
		if (fix)
		{
			const auto computed_fix = reset::index(rfr, rfr_detail, d, indx_detail);
			if (*fix != computed_fix)
				std::cout
					<< std::fixed
					<< std::setprecision(indx.get_decimal_places())
					<< "For "
					<< d
					<< " "
					<< indx_label
					<< " is "
					<< fix->get_value()
					<< " and the same computed value is "
					<< computed_fix.get_value()
					<< std::endl;
		}
	}
}


template<auto Average = reset::average, int ShiftDays = 0> // Average is a callable used to compute the compounded average rate; ShiftDays is days to shift business days
[[nodiscard]] auto make_compounded_average_check_task(
	const reset::RateFixings& rfr,
	const reset::rate_fixings_detail& rfr_detail,
	const reset::RateFixings& avg, // might need a better name, as it is compounded average fixings, not a simple average
	const reset::average_detail& avg_detail,
	const std::string& avg_label
)
{
	return std::async(std::launch::async, [&rfr, &rfr_detail, &avg, &avg_detail, &avg_label]() {
		const auto& cal = rfr.get_calendar(); // we can assert that avg and rfr have the same calendar
		const auto dates = cal.make_business_days_schedule(avg.get_time_series().get_period());
		for (const auto& dt : dates.get_dates())
		{
			const auto& observed = avg[dt];
			assert(observed);

			const auto dt_shifted = gregorian::shift_business_days(dt, std::chrono::days{ ShiftDays }, cal); // other indices might have a different convention for RRF date vs avg date
			const auto calculated = Average(rfr, rfr_detail, dt_shifted, avg_detail).percent;

			if (*observed != calculated)
			{
				auto scout = std::osyncstream{ std::cout };
				scout
					<< std::fixed
					<< std::setprecision(avg.get_decimal_places())
					<< "For "
					<< dt_shifted
					<< " "
					<< avg_label
					<< " is "
					<< observed->get_value()
					<< " and the same computed value is "
					<< calculated.get_value()
					<< std::endl;
			}
		}
	});
}
