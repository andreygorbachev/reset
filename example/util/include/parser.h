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
#include <climits>


struct parser_detail // should it be called metadata?
{
	const unsigned int header_lines = 1u; // how many lines to skip at the beginning of the file
	const std::chrono::year_month_day& from; // this could also be read from the file
	const std::chrono::year_month_day& until; // this could also be read from the file
	const std::string date_format; // should we default it to ISO?
	const char separator = ','; // the character that separates columns in the file
	const char padder = ' '; // the character that pads observation columns in the file
	const unsigned int skip_columns = 0u; // how many columns to skip after the date column to get to the required observation column
};



inline auto _parse_date(
	std::istream& fs,
	const parser_detail& detail
)
{
	auto ymd = std::chrono::year_month_day{};

#ifdef _MSC_BUILD 
	std::chrono::from_stream(fs, detail.date_format.c_str(), ymd);
#else
	throw std::domain_error{ "Not implemented" };
#endif

	fs.ignore(1, detail.separator);

	return ymd;
}

template<typename Fixings>
auto _parse_observation(
	std::istream& fs,
	const parser_detail& detail
)
{
	fs.ignore(1, detail.padder);

	auto o = std::string{};
	std::getline(fs, o, detail.separator);

	// we do not bother with detail.separator here as the rest of this line will be ignored anyway

	return typename Fixings::observation{ o };
}



template<typename Fixings>
auto _parse_csv_fixings_storage(
	std::istream& fs,
	const parser_detail& detail
)
{
	auto result = typename Fixings::storage{ gregorian::util::days_period{ detail.from, detail.until } };

	while (!fs.eof())
	{
		const auto ymd = _parse_date(fs, detail);

		for (auto i = 0u; i < detail.skip_columns; ++i)
			fs.ignore(INT_MAX, detail.separator);

		result[ymd] = _parse_observation<Fixings>(fs, detail);

		fs.ignore(INT_MAX, '\n');
	}

	return result;
}



template<typename Fixings>
auto parse_csv_fixings(
	const std::string& fileName,
	const parser_detail& detail
) -> Fixings
{
	/*const*/ auto fs = std::ifstream{ fileName }; // should we handle a default .csv file extension?
	assert(fs);

	// skip the header
	for (auto i = 0u; i < detail.header_lines; ++i)
		fs.ignore(INT_MAX, '\n');

	auto ts = _parse_csv_fixings_storage<Fixings>(fs, detail);
	// we can check the fixings vs decimal places

	return Fixings{
		std::move(ts),
		gregorian::static_data::locate_calendar(
			"Europe/Zurich", // is this a sperate parameter? or a detail of the file?
			detail.until
		),
		6u // is this a sperate parameter? or a detail of the file?
	};
}
