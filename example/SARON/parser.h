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

#include <resets.h>

#include <period.h>

#include <day_count.h>
#include <actual_360.h>

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

	using namespace std::string_literals;

	return reset::resets::observation{ o };
}

inline auto _parse_observation2(std::istream& fs)
{
	auto o = std::string{};
	std::getline(fs, o);

	using namespace std::string_literals;

	return reset::resets::observation{ o };
}


inline auto _parse_csv_resets_storage_x2(
	std::istream& fs,
	const std::chrono::year_month_day& from, // these could also be read from the file
	const std::chrono::year_month_day& until
) -> std::tuple<reset::resets::storage, reset::resets::storage>
{
	auto result1 = reset::resets::storage{ gregorian::days_period{ from, until } };
	auto result2 = reset::resets::storage{ gregorian::days_period{ from, until } };

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

	return { result1, result2 };
}


inline auto _make_calendar(const reset::resets::storage& ts)
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


inline auto parse_csv_resets_x2(
	const std::string& fileName,
	const std::chrono::year_month_day& from, // these could also be read from the file
	const std::chrono::year_month_day& until
) -> std::tuple<reset::resets, reset::resets> // SARON, SARON Compounded Index
{
	/*const*/ auto fs = std::ifstream{ fileName }; // should we handle a default .csv file extension?

	// skip the first line (header)
	auto t = std::string{};
	std::getline(fs, t);

	auto [ts1, ts2] = _parse_csv_resets_storage_x2(fs, from, until);

	auto c = _make_calendar(ts1);

	const auto dc = fin_calendar::actual_360<boost::multiprecision::cpp_dec_float_50>{};

	return { reset::resets{ std::move(ts1), c, dc }, reset::resets{ std::move(ts2), c, dc } };
}
