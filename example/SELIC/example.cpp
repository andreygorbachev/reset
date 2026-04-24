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

#include <fixings.h>
#include <index.h>

#include <calculation_252.h>

#include <chrono>
#include <iostream>
#include <iomanip>
#include <ios>

using namespace std;
using namespace std::chrono;

using namespace boost::multiprecision;

using namespace fin_calendar;

using namespace reset;



static auto parse_csv_fixings_SELIC() -> RateFixings
{
	// from https://api.bcb.gov.br/dados/serie/bcdata.sgs.1178/dados?formato=csv&dataInicial=01/07/2000&dataFinal=dd/mm/yyyy
	return parse_csv_fixings(
		"SELIC Effective.csv",
		2000y / July / 1d,
		2014y / July / 1d
	);
}



int main()
{
	const auto SELIC = parse_csv_fixings_SELIC();

	auto rfd = rate_fixing_detail{};
	rfd.day_count = calculation_252<cpp_dec_float_50>{ SELIC.get_calendar() }; // think more about copies of calendar

	auto id = index_detail{};
	id.initial_value = cpp_dec_float_50{ 1000 };
	id.initial_date = 2000y / July / 1d;
	id.brazil = true;
	id.factor_round = 8u;
	id.final_trunc = 6u;

	// from https://wilsonfreitas.github.io/posts/pricing-brazilian-government-bonds-lft.html

	const auto date1 = 2014y / March / 21d;

	cout
		<< fixed
		<< setprecision(id.final_trunc.value())
		<< "For "
		<< date1
		<< " VNA is "
		<< cpp_dec_float_50{ "6023.149269" }
		<< " and the same computed value is "
		<< index(SELIC, rfd, date1, id).get_value()
		<< endl;

	// from "Metodologia de Cálculo dos Títulos Públicos Federais Ofertados nos Leilões Primários"
	// and "Methodology for Calculating Federal Government Bonds Offered in Primary Auctions"

	const auto date2 = 2008y / May / 20d; // but computation for 21'st uses SELIC Target rather than SELIC Effective, which is probably wrong

	cout
		<< fixed
		<< setprecision(id.final_trunc.value())
		<< "For "
		<< date2
		<< " VNA is "
		<< cpp_dec_float_50{ "3449.694215" } // for some reason the English version of the same document has different values
		<< " and the same computed value is "
		<< index(SELIC, rfd, date2, id).get_value()
		<< endl;

	return 0;
}
