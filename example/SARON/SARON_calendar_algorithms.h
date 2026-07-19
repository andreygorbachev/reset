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

#include <preceding.h>

#include <calendar.h>



// should these 2 functions be in calendar? (or if they are more finance specific in fin-calendar/util?)
// an appropriate namespace should be also used

inline auto get_last_business_day_of_month(
	const std::chrono::year_month& ym,
	const gregorian::calendar& cal
) -> std::chrono::year_month_day
{
	constexpr auto preceding = fin_calendar::preceding{};
	const auto candidate = std::chrono::year_month_day{ ym / std::chrono::last };
	return preceding.adjust(candidate, cal);
}

inline auto is_last_business_day_of_month(
	const std::chrono::year_month_day& ymd,
	const gregorian::calendar& cal
) -> bool
{
	return ymd == get_last_business_day_of_month(ymd.year() / ymd.month(), cal);
}
