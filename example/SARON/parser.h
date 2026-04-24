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

#include <scaled_value.h>
#include <fixings.h>

#include <period.h>

#include <string>
#include <chrono>
#include <istream>
#include <fstream>
#include <stdexcept>
#include <utility>
#include <tuple>


inline auto _parse_date(std::istream& fs)
{
	auto ymd = std::chrono::year_month_day{};

#ifdef _MSC_BUILD 
	std::chrono::from_stream(fs, "%d/%m/%Y", ymd);
#else
	throw std::domain_error{ "Not implemented" };
#endif

	return ymd;
}

inline auto _parse_observation1(std::istream& fs)
{
	auto o = std::string{};
	std::getline(fs, o, ','); // can we make _parse_observation1 and _parse_observation2 the same (and parse the comma in the caller)?
	// should we check decimal places?

	return reset::Percent{ o };
}

inline auto _parse_observation2(std::istream& fs)
{
	auto o = std::string{};
	std::getline(fs, o);
	// should we check decimal places?

	return reset::Value{ o };
}


inline auto _parse_csv_fixings_storage_x2(
	std::istream& fs,
	const std::chrono::year_month_day& from, // these could also be read from the file
	const std::chrono::year_month_day& until
)
{
	auto result1 = reset::RateFixings::storage{ gregorian::util::days_period{ from, until } };
	auto result2 = reset::IndexFixings::storage{ gregorian::util::days_period{ from, until } };

	for (;;)
	{
		const auto ymd = _parse_date(fs);

		auto s1 = std::string{};
		std::getline(fs, s1, ','); // skip the comma

		const auto observation1 = _parse_observation1(fs);

		result1[ymd] = observation1;

		const auto observation2 = _parse_observation2(fs);

		result2[ymd] = observation2;

		if (fs.eof())
			break;
	}

	return std::tuple{ result1, result2 };
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


inline auto parse_csv_fixings_x2(
	const std::string& fileName,
	const std::chrono::year_month_day& from, // these could also be read from the file
	const std::chrono::year_month_day& until
) -> std::tuple<reset::RateFixings, reset::IndexFixings> // SARON, SARON Compounded Index
{
	/*const*/ auto fs = std::ifstream{ fileName }; // should we handle a default .csv file extension?

	// skip the first line (header)
	auto t = std::string{};
	std::getline(fs, t);

	auto [ts1, ts2] = _parse_csv_fixings_storage_x2(fs, from, until);

	auto c1 = _make_calendar<reset::RateFixings>(ts1);
	auto c2 = _make_calendar<reset::IndexFixings>(ts2);

	return {
		reset::RateFixings{ std::move(ts1), std::move(c1), 6 },
		reset::IndexFixings{ std::move(ts2), std::move(c2), 5 }
	}; // index needs to be checked a bit more
}
