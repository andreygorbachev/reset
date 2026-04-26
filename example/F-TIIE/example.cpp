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



static auto parse_csv_fixings_FTIIE() -> RateFixings
{
	// from https://www.banxico.org.mx/SieInternet/consultarDirectorioInternetAction.do?&sector=18&accion=consultarDirectorioCuadros&locale=en
	return parse_csv_fixings<RateFixings>(
		"Overnight TIIE Funding Rate.csv",
		2u,
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

	const auto FTIIE_compounded_index = parse_csv_fixings_FTIIE_compounded_index();

	return 0;
}
