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

#include <parser.h>

#include <fixings.h>

#include <chrono>
#include <optional>

using namespace std;
using namespace std::chrono;

using namespace gregorian;
using namespace gregorian::util;
using namespace gregorian::static_data;

using namespace fin_calendar;

using namespace reset;



static auto parse_csv_fixings_SHIR() -> RateFixings
{
	// from https://edge.boi.gov.il/FusionDataBrowser/?df=BOI.STATISTICS:BR(1.0)

	constexpr auto d = parser_detail{
		.header_lines = 5u,
		.from = 2022y / May / 2d,
		.until = 2026y / May / 21d,
		.date_format = "%d/%m/%Y",
		.separator = ',',
		.padder = nullopt,
		.not_available = nullopt,
		.skip_columns = 2u
	};

	return parse_csv_fixings<RateFixings>(
		"BR.csv",
		d,
		"Asia/SHIR",
		2u // not sure if that is correct
	);
}



int main()
{
	const auto SHIR = parse_csv_fixings_SHIR();

	return 0;
}
