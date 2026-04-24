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

#include <fixings.h>

#include <calendar.h>
#include <schedule.h>
#include <period.h>
#include <weekend.h>

#include <string>
#include <chrono>
#include <istream>
#include <fstream>
#include <stdexcept>
#include <utility>
#include <optional>


inline auto _parse_date(std::istream& fs)
{
	auto ymd = std::chrono::year_month_day{};

#ifdef _MSC_BUILD 
	std::chrono::from_stream(fs, "\"%d %b %y\"", ymd);
#else
	throw std::domain_error{ "Not implemented" };
#endif

	return ymd;
}

template<typename Fixings>
auto _parse_observation(std::istream& fs)
{
	auto o = std::string{};
	std::getline(fs, o);

	using namespace std::string_literals;

	return typename Fixings::observation{
		o.substr(1uz, o.length() - 2uz)
	}; // we ignore the first and last characters (quotes)
}


template<typename Fixings>
auto _parse_csv_fixings_storage(
	std::istream& fs,
	const std::chrono::year_month_day& from, // these could also be read from the file
	const std::chrono::year_month_day& until
)
{
	auto result = typename Fixings::storage{ gregorian::util::days_period{ from, until } };

	for (;;)
	{
		const auto ymd = _parse_date(fs);

		auto s = std::string{};
		std::getline(fs, s, ','); // skip the comma
		// check decimal places?

		const auto observation = _parse_observation<Fixings>(fs);

		result[ymd] = observation;

		if (fs.eof())
			break;
	}

	return result;
}


template<typename Fixings>
auto _make_calendar(const typename Fixings::storage& ts)
{
	const auto& fu = ts.get_period();

	auto hols = gregorian::schedule::dates{};
	for (
		auto d = fu.get_from();
		d <= fu.get_until();
		d = std::chrono::sys_days{ d } + std::chrono::days{ 1 }
	)
		if (ts[d] == std::nullopt)
			hols.insert(d);

	return gregorian::calendar{
		gregorian::NoWeekend,
		gregorian::schedule{ fu, hols }
	};
}


template<typename Fixings>
auto parse_csv_fixings(
	const std::string& fileName,
	const std::chrono::year_month_day& from, // these could also be read from the file
	const std::chrono::year_month_day& until,
	const int dp
) -> Fixings
{
	/*const*/ auto fs = std::ifstream{ fileName }; // should we handle a default .csv file extension?

	// skip the first line (header)
	auto t = std::string{};
	std::getline(fs, t);

	auto ts = _parse_csv_fixings_storage<Fixings>(fs, from, until);

	auto c = _make_calendar<Fixings>(ts);

	return Fixings{ std::move(ts), std::move(c), dp };
}
