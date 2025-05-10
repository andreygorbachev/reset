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

#include <day_count.h>

#include <time_series.h>

#include <cmath>
#include <memory>
#include <optional>
#include <stdexcept>


namespace risk_free_rate
{

	constexpr auto from_percent(const double val) noexcept -> double // is there a standard way to do this?
	{
		return val / 100.0;
	}

	constexpr auto to_percent(const double val) noexcept -> double // is there a standard way to do this?
	{
		return val * 100.0;
	}



	class resets
	{

	public:

		using storage = gregorian::_time_series<std::optional<double>>; // or should we consider some ratio? (s.t. rounding would be explicit)

	public:

		resets() noexcept = delete;
		resets(const resets&) = default;
		resets(resets&&) noexcept = default;

		~resets() noexcept = default;

		resets& operator=(const resets&) = default;
		resets& operator=(resets&&) noexcept = default;

		explicit resets(storage ts, fin_calendar::day_count<double> dc); // why does it not use default?

	public:

		auto operator[](const std::chrono::year_month_day& ymd) const -> double;
		// this also converts from percentages and throws an exception for missing resets - is it what we want?


	public:

		auto get_time_series() const noexcept -> const storage&;
		auto get_day_count() const noexcept -> const fin_calendar::day_count<double>&;

	public:

		auto last_reset_year_month_day() const noexcept -> std::chrono::year_month_day;

	private:

		storage ts_;

		fin_calendar::day_count<double> dc_;

	};



	inline resets::resets(storage ts, fin_calendar::day_count<double> dc) :
		ts_{ std::move(ts) },
		dc_{ dc }
	{
	}



	inline auto resets::operator[](const std::chrono::year_month_day& ymd) const -> double
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

	inline auto resets::get_day_count() const noexcept -> const fin_calendar::day_count<double>&
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
