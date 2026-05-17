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

#include <static_data.h>

#include <period.h>

#include <string>
#include <chrono>
#include <istream>
#include <fstream>
#include <stdexcept>
#include <utility>
#include <cassert>


inline auto _parse_date(std::istream& fs)
{
	auto ymd = std::chrono::year_month_day{};

#ifdef _MSC_BUILD 
	std::chrono::from_stream(fs, "%d.%m.%Y", ymd);
#else
	throw std::domain_error{ "Not implemented" };
#endif

	auto skip = std::string{};
	std::getline(fs, skip, ';');

	return ymd;
}

template<typename Fixings>
auto _parse_observation(std::istream& fs)
{
	auto skip = std::string{};
	std::getline(fs, skip, ' ');

	auto o = std::string{};
	std::getline(fs, o, ';');

	return typename Fixings::observation{ o };
}


template<typename Fixings>
auto _parse_csv_fixings_storage(
	std::istream& fs,
	const unsigned int skip,
	const std::chrono::year_month_day& from, // these could also be read from the file
	const std::chrono::year_month_day& until
)
{
	auto result = typename Fixings::storage{ gregorian::util::days_period{ from, until } };

	while (!fs.eof())
	{
		const auto ymd = _parse_date(fs);

		auto s = std::string{};
		for (auto i = 0u; i < skip; ++i)
			std::getline(fs, s, ';');

		result[ymd] = _parse_observation<Fixings>(fs);

		std::getline(fs, s);
	}

	return result;
}


template<typename Fixings>
auto parse_csv_fixings(
	const std::string& fileName,
	const unsigned int skip, // how many columns to skip after date before observation
	const std::chrono::year_month_day& from, // these could also be read from the file
	const std::chrono::year_month_day& until
) -> Fixings
{
	/*const*/ auto fs = std::ifstream{ fileName }; // should we handle a default .csv file extension?
	assert(fs);

	// skip the first 4 lines (header)
	auto s = std::string{};
	std::getline(fs, s);
	std::getline(fs, s);
	std::getline(fs, s);
	std::getline(fs, s);

	auto ts = _parse_csv_fixings_storage<Fixings>(fs, skip, from, until);
	// we can check the fixings vs decimal places

	return Fixings{
		std::move(ts),
		gregorian::static_data::locate_calendar(
			"Europe/Zurich",
			std::chrono::year{ 2026 } / std::chrono::May / std::chrono::day{ 12u }
		),
		6u
	};
}
