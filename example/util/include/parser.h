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

#include <calendar.h>
#include <schedule.h>
#include <weekend.h>
#include <period.h>

#include <string>
#include <chrono>
#include <istream>
#include <fstream>
#include <stdexcept>
#include <utility>
#include <cassert>
#include <climits>
#include <optional>
#include <algorithm>



struct parser_detail // should it be called metadata?
{
	unsigned int header_lines = 1u; // how many lines to skip at the beginning of the file
	std::chrono::year_month_day from; // this could also be read from the file
	std::chrono::year_month_day until; // this could also be read from the file
	std::string date_format; // should we default it to ISO?
	char separator = ','; // the character that separates columns in the file
	std::optional<char> padder; // the character that pads observation columns in the file
	std::optional<std::string> not_available; // the string that indicates that the observation is not available
	unsigned int skip_columns = 0u; // how many columns to skip after the date column to get to the required observation column
};

template<typename Fixings>
auto parse_csv_fixings(
	const std::string& fileName,
	const parser_detail& detail,
	const std::optional<std::string> calendar_name, // empty calendar will be created if this is nullopt
	typename const Fixings::decimal_places decimal_places
) -> Fixings;



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
) -> std::optional<typename Fixings::observation>
{
	if (detail.padder)
		fs.ignore(1, *detail.padder);

	auto o = std::string{};
	std::getline(fs, o);

	if (detail.not_available && o.starts_with(*detail.not_available))
		return std::nullopt;

	const auto comma_pos = o.find(detail.separator);
	if (comma_pos != std::string::npos)
		// not the last column
		return typename Fixings::observation{
			o.substr(0uz, comma_pos)
		};
	else
		// the last column
		return typename Fixings::observation{
			o
		};
}


template<typename Fixings>
auto _parse_csv_fixings_storage(
	std::istream& fs,
	const parser_detail& detail
)
{
	auto result = typename Fixings::storage{ gregorian::util::days_period{ detail.from, detail.until } };

	const auto period = gregorian::util::days_period{ detail.from, detail.until };

	while (!fs.eof())
	{
		const auto ymd = _parse_date(fs, detail);

		if (period.contains(ymd))
		{
			for (auto i = 0u; i < detail.skip_columns; ++i)
				fs.ignore(INT_MAX, detail.separator);

			result[ymd] = _parse_observation<Fixings>(fs, detail);
		}
		else
		{
			fs.ignore(INT_MAX, '\n');
		}
	}

	return result;
}


inline auto _make_empty_calendar(const parser_detail& detail)
{
	return gregorian::calendar{
		gregorian::NoWeekend,
		gregorian::schedule{
			gregorian::util::days_period{ detail.from, detail.until },
			{}
		}
	};
}



template<typename Fixings>
auto parse_csv_fixings(
	const std::string& fileName,
	const parser_detail& detail,
	const std::optional<std::string> calendar_name,
	typename const Fixings::decimal_places decimal_places
) -> Fixings
{
	/*const*/ auto fs = std::ifstream{ fileName }; // should we handle a default .csv file extension?
	assert(fs);

	// skip the header
	for (auto i = 0u; i < detail.header_lines; ++i)
		fs.ignore(INT_MAX, '\n');

	auto ts = _parse_csv_fixings_storage<Fixings>(fs, detail);
	// we can check the fixings vs decimal places

	auto calendar =
		calendar_name ?
		gregorian::static_data::locate_calendar(*calendar_name,	detail.until) :
		_make_empty_calendar(detail);

	return Fixings{
		std::move(ts),
		std::move(calendar),
		decimal_places
	};
}
