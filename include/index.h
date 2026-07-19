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

#include <ranges>
#include <optional>
#include <chrono>
//#include <cassert>

#include <boost/decimal.hpp>

#include <period.h>

#include <calendar.h>

#include <day_count.h>

#include "fixings.h"
#include "reset_math.h"
#include "scaled_value.h"


namespace reset
{

	struct index_detail // should it be called metadata?
	{
		Value initial_value = Value{ "1" }; // is this what we want? (should it be left uninitialised like the intial_date?
		std::chrono::year_month_day initial_date;
		bool brazil = false; // this needs to be better - maybe "calendar"/"business" compounding enum?
		std::optional<unsigned int> factor_trunc;
		std::optional<unsigned int> factor_round;
		std::optional<unsigned int> step_trunc;
		std::optional<unsigned int> step_round;
		std::optional<unsigned int> final_trunc;
		std::optional<unsigned int> final_round; // should roundings and truncations be int?
		std::optional<gregorian::calendar> calendar; // does it need to be optional? does it need to be a copy? do we need it at all? (could be lifted from fixings)
	};


	void index_step_(
		boost::decimal::decimal128_t& indx,
		const std::chrono::year_month_day& start,
		const std::chrono::year_month_day& end,
		const RateFixings& fix,
		const rate_fixings_detail& rfd,
		const index_detail& id
	);


	// maybe this needs a better name? - compute a compounded RFR index from the underlying fixings
	[[nodiscard]]inline auto index(
		const RateFixings& fix,
		const rate_fixings_detail& rfd,
		const std::chrono::year_month_day& ymd,
		const index_detail& id = index_detail{} // does it need a default?
	) -> Value
	{
		// should throw an exception if we requested an index on a business day before the first reset
		// but we do not have information about relevant calendar at the moment

		const auto& c = id.calendar ?
			*id.calendar :
			fix.get_calendar();
		const auto schedule = c.make_business_days_schedule(
			gregorian::util::days_period{ id.initial_date, ymd }
		); // is this a wrong data structure?
		// assert that it is not empty?

		const auto& dates = schedule.get_dates();

		auto indx = static_cast<boost::decimal::decimal128_t>(id.initial_value); // or should intial_value not be a Value? (we do not need to convert it to decimal128_t if it is a Value)

		for (const auto& [start, end] : dates | std::views::adjacent<2uz>)
			index_step_(indx, start, end, fix, rfd, id);

		if (id.final_trunc)
			indx = trunc_dp(indx, *id.final_trunc);

		if (id.final_round)
			indx = round_dp(indx, *id.final_round);

		return indx; // no conversion needed?
	}

// we can also compute all the index values at the same time for all publication dates up to ymd
// adjusent_difference for year fraction
// partial_sum for daily compounding
// transform for final rounding


	inline auto index_factor_(
		const std::chrono::year_month_day& start,
		const std::chrono::year_month_day& end,
		const RateFixings& fix,
		const rate_fixings_detail& rfd,
		const index_detail& detail
	)
	{
		using namespace boost::decimal::literals;

//		const auto& fixing = fix[start];
//		assert(fixing);
//		const auto rate = static_cast<Decimal>(*fixing);
		const auto& fixing = fix.with_fallback(start);
		const auto rate = static_cast<boost::decimal::decimal128_t>(fixing);

		const auto year_fraction = fin_calendar::fraction(start, end, rfd.day_count);

		auto factor = detail.brazil ?
			pow(1_dl + rate, year_fraction) : // we are also missing rounding for Brazil year_fraction at the moment
			1_dl + rate * year_fraction; // should these have some kind of units?

		if (detail.factor_trunc)
			factor = trunc_dp(factor, *detail.factor_trunc);

		if (detail.factor_round)
			factor = round_dp(factor, *detail.factor_round);

		return factor;
	}

	inline void index_step_(
		boost::decimal::decimal128_t& indx,
		const std::chrono::year_month_day& start,
		const std::chrono::year_month_day& end,
		const RateFixings& fix,
		const rate_fixings_detail& rfd,
		const index_detail& id
	)
	{
		indx *= index_factor_(start, end, fix, rfd, id);

		if (id.step_trunc)
			indx = trunc_dp(indx, *id.step_trunc);

		if (id.step_round)
			indx = round_dp(indx, *id.step_round);
	}

}
