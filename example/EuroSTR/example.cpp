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

#include "parser.h"

#include <decimal.h>
#include <fixings.h>

#include <chrono>

using namespace std;
using namespace std::chrono;

using namespace fin_calendar;

using namespace reset;



static auto parse_csv_fixings_EuroSTR() -> RateFixings
{
	// from https://www.ecb.europa.eu/stats/financial_markets_and_interest_rates/euro_short-term_rate/html/index.en.html
	return parse_csv_fixings(
		"Euro short-term rate.csv",
		2019y / October / 1d,
		2026y / April / 23d
	);
}

static auto parse_csv_fixings_PreEuroSTR() -> RateFixings
{
	// from https://www.ecb.europa.eu/stats/financial_markets_and_interest_rates/euro_short-term_rate/html/index.en.html
	return parse_csv_fixings(
		"Pre-Euro short-term rate.csv",
		2017y / March / 15d,
		2019y / September / 30d
	);
}



int main()
{
	const auto EuroSTR = parse_csv_fixings_EuroSTR();
	const auto PreEuroSTR = parse_csv_fixings_PreEuroSTR();

	// how can we "add" these 2 together?

	return 0;
}
