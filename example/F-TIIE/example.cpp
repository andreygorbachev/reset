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
#include <index.h>
#include <fixings.h>

#include <actual_360.h>

#include <chrono>
#include <iostream>
#include <iomanip>
#include <ios>
#include <cassert>

using namespace std;
using namespace std::chrono;

using namespace fin_calendar;

using namespace reset;



static auto parse_csv_fixings_FTIIE() -> RateFixings
{
	// from https://www.banxico.org.mx/SieInternet/consultarDirectorioInternetAction.do?&sector=18&accion=consultarDirectorioCuadros&locale=en
	return parse_csv_fixings<RateFixings>(
		"Overnight TIIE Funding Rate.csv",
		0u,
		2006y / January / 2d,
		2026y / April / 24d,
		2u
	);
}

static auto parse_csv_fixings_FTIIE_compounded_index() -> IndexFixings
{
	// from https://www.banxico.org.mx/SieInternet/consultarDirectorioInternetAction.do?sector=18&accion=consultarCuadroAnalitico&idCuadro=CA766&locale=en
	return parse_csv_fixings<IndexFixings>(
		"Overnight Funding TIIE indexes and compounded in advance Overnight Funding TIIE.csv",
		1u,
		2006y / January / 2d,
		2026y / April / 27d,
		4u
	);
}



int main()
{
	const auto FTIIE = parse_csv_fixings_FTIIE();

	auto rfd = rate_fixing_detail{};
	rfd.day_count = actual_360<Decimal>{};

	const auto FTIIE_compounded_index = parse_csv_fixings_FTIIE_compounded_index();

	// from
	// "Determination of the Overnight Funding TIIE Index compounded on business days,
	// the Overnight Funding TIIE Index compounded on calendar days,
	// and the Compounded in advance Overnight Funding TIIE."
	auto id = index_detail{};
	id.initial_value = Decimal{ 100'000 };
	id.initial_date = 2006y / January / 2d;
	id.step_round = 16u;
	id.final_round = 4u;

//	const auto date = 2026y / April / 27d;
	const auto date = 2026y / April / 24d;

	const auto& indx = FTIIE_compounded_index[date];
	assert(indx);

	cout
		<< fixed
		<< setprecision(FTIIE_compounded_index.get_decimal_places())
		<< "For "
		<< date
		<< " F-TIIE Compounded Index (business days) is "
		<< indx->get_value()
		<< " and the same computed value is "
		<< index(FTIIE, rfd, date, id).get_value()
		<< endl;

	return 0;
}
