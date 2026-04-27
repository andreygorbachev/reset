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

#include "decimal.h"
#include "scaled_value.h"

#include <day_count.h>
#include <preceding.h>

#include <time_series.h>

#include <calendar.h>

#include <utility>
#include <chrono>
#include <optional>


namespace reset
{

	struct rate_fixing_detail // should it be called metadata?
	{
		fin_calendar::day_count<Decimal> day_count;
	};



	template<typename Observation>
	class fixings // at the moment we are not thinking about revisions
	{

	public:

		using observation = Observation;

		using storage = gregorian::util::time_series<std::optional<observation>>; // this is for value dates not publication dates

		using calendar = gregorian::calendar;

		using decimal_places = unsigned int; // static_assert consistency between cpp_dec_float_50 and unsigned int?

	public:

		explicit fixings(storage ts, calendar c, decimal_places dp);

	public:

		auto operator[](const std::chrono::year_month_day& ymd) const -> const std::optional<observation>&;

		auto fallback(const std::chrono::year_month_day& ymd) const -> bool; // is it a good name?
		// what should it do if ymd is not a good business day?

		// do we need another function which would check if fixing is not expected, but is provided?

		auto current_observation(const std::chrono::year_month_day& ymd) const -> const observation&; // is it a good name?
		// throws an exception when we are pushed before the start of fixings

	public:

		auto get_time_series() const noexcept -> const storage&;
		auto get_calendar() const noexcept -> const calendar&;
		auto get_decimal_places() const noexcept -> decimal_places;

	public:

		auto last_reset_year_month_day() const noexcept -> std::chrono::year_month_day;

	private:

		storage ts_;

		calendar c_; // do we want to copy the calendar?

		decimal_places dp_;

	};



	using RateFixings = fixings<Percent>;
	using IndexFixings = fixings<Value>;



	template<typename Observation>
	fixings<Observation>::fixings(storage ts, calendar c, decimal_places dp) :
		ts_{ std::move(ts) },
		c_{ std::move(c) },
		dp_{ dp }
	{
	}


	template<typename Observation>
	auto fixings<Observation>::operator[](const std::chrono::year_month_day& ymd) const -> const std::optional<observation>&
	{
		return ts_[ymd];
	}

	template<typename Observation>
	auto fixings<Observation>::fallback(const std::chrono::year_month_day& ymd) const -> bool
	{
		return !(c_.is_business_day(ymd) && ts_[ymd]);
	}

	template<typename Observation>
	auto fixings<Observation>::current_observation(const std::chrono::year_month_day& ymd) const -> const observation&
	{
		static const auto p = fin_calendar::preceding{};
		const auto& o = ts_[p.adjust(ymd, c_)];
		if (o)
			return *o;
		else
			return current_observation(std::chrono::sys_days{ ymd } - std::chrono::days{ 1 });
	}


	template<typename Observation>
	auto fixings<Observation>::get_time_series() const noexcept -> const storage&
	{
		return ts_;
	}

	template<typename Observation>
	auto fixings<Observation>::get_calendar() const noexcept -> const calendar&
	{
		return c_;
	}

	template<typename Observation>
	auto fixings<Observation>::get_decimal_places() const noexcept -> decimal_places
	{
		return dp_;
	}


	template<typename Observation>
	auto fixings<Observation>::last_reset_year_month_day() const noexcept -> std::chrono::year_month_day
	{
		auto result = ts_.get_period().get_until();
		while (!ts_[result])
			result = std::chrono::sys_days{ result } - std::chrono::days{ 1 };

		return result;
	}

}
