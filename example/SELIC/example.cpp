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

#include <resets.h>
#include <index.h>

#include <chrono>
#include <iostream>
#include <iomanip>

using namespace std;
using namespace std::chrono;

using namespace boost::multiprecision;

using namespace reset;



inline auto parse_csv_resets_SELIC() -> resets
{
	// from https://api.bcb.gov.br/dados/serie/bcdata.sgs.1178/dados?formato=csv&dataInicial=01/07/2000&dataFinal=dd/mm/yyyy
	return parse_csv_resets(
		"..\\..\\..\\example\\SELIC\\data\\SELIC Effective.csv",
		2000y / July / 1d,
		2014y / July / 1d
	); // or set a working directory?
}



int main()
{
	const auto SELIC = parse_csv_resets_SELIC();

	auto detail = index_detail{};
	detail.initial_value = cpp_dec_float_50{ 1000 };
	detail.initial_date = 2000y / July / 1d;
	detail.brazil = true;
	detail.factor_round = 8u;
	detail.final_trunc = 6u;

	// from https://wilsonfreitas.github.io/posts/pricing-brazilian-government-bonds-lft.html

	const auto date1 = 2014y / March / 21d;

	cout
		<< fixed
		<< setprecision(6)
		<< "For "
		<< date1
		<< " VNA is "
		<< cpp_dec_float_50{ "6023.149269" }
		<< " and the same computed value is "
		<< index(SELIC, date1, detail)
		<< endl;

	// from "Metodologia de Cálculo dos Títulos Públicos Federais Ofertados nos Leilões Primários"

	const auto date2 = 2008y / May / 20d; // but computation for 21'st uses SELIC Target rather than SELIC Effective, which is probably wrong

	cout
		<< fixed
		<< setprecision(6)
		<< "For "
		<< date2
		<< " VNA is "
		<< cpp_dec_float_50{ "3449.694215" } // for some reason the English version of the same document has different values
		<< " and the same computed value is "
		<< index(SELIC, date2, detail)
		<< endl;

	return 0;
}
