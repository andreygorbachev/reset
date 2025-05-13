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

#include "resets_math.h"

#include <day_count.h>

#include <time_series.h>
#include <calendar.h>

#include <boost/multiprecision/cpp_dec_float.hpp>

#include <cmath>
#include <memory>
#include <optional>
#include <stdexcept>


namespace reset
{

	class resets
	{

	public:

		using observation = boost::multiprecision::cpp_dec_float_50;

		using storage = gregorian::_time_series<std::optional<observation>>; // this is for value dates not publication dates

		using calendar = gregorian::calendar;

		using day_count = fin_calendar::day_count<boost::multiprecision::cpp_dec_float_50>;

	public:

		explicit resets(storage ts, calendar c, day_count dc);

	public:

		auto operator[](const std::chrono::year_month_day& ymd) const -> observation;
		// this converts from percentages
		// and throws an exception for missing resets - is it what we want?
		// what should we do for the resets which meant to be published but are not available (and the prevous business day resets should be used instead)?

	public:

		auto get_time_series() const noexcept -> const storage&;
		auto get_calendar() const noexcept -> const calendar&;
		auto get_day_count() const noexcept -> const day_count&;

	public:

		auto last_reset_year_month_day() const noexcept -> std::chrono::year_month_day;

	private:

		storage ts_;

		calendar c_;

		day_count dc_; // is this the right place for this? (does SONIA compounded index has a day count?)

	};



	inline resets::resets(storage ts, calendar c, day_count dc) :
		ts_{ std::move(ts) },
		c_{ std::move(c) },
		dc_{ dc }
	{
	}



	inline auto resets::operator[](const std::chrono::year_month_day& ymd) const -> observation
	{
		const auto& o = ts_[ymd];
		if (o)
			return from_percent(*o);
		else
			throw std::out_of_range{ "Request is not consistent with publication calendar" };
	}


	inline auto resets::get_time_series() const noexcept -> const storage&
	{
		return ts_;
	}

	inline auto resets::get_calendar() const noexcept -> const calendar&
	{
		return c_;
	}

	inline auto resets::get_day_count() const noexcept -> const day_count&
	{
		return dc_;
	}


	inline auto resets::last_reset_year_month_day() const noexcept -> std::chrono::year_month_day
	{
		auto result = ts_.get_period().get_until();
		while (!ts_[result])
			result = std::chrono::sys_days{ result } - std::chrono::days{ 1 };

		return result;
	}

}
