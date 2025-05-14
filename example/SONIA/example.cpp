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

using namespace std;
using namespace std::chrono;

using namespace boost::multiprecision;

using namespace reset;



inline auto parse_csv_resets_SONIA() -> resets
{
	// from https://www.bankofengland.co.uk/markets/sonia-benchmark
	return parse_csv_resets(
		"..\\..\\..\\example\\SONIA\\data\\SONIA.csv",
		1997y / January / 1d,
		2025y / May / 13d
	); // or set a working directory?
}

inline auto parse_csv_resets_SONIA_compounded_index() -> resets
{
	// from https://www.bankofengland.co.uk/markets/sonia-benchmark
	return parse_csv_resets(
		"..\\..\\..\\example\\SONIA\\data\\SONIA Compounded Index.csv",
		2018y / April / 23d,
		2025y / May / 13d
	); // or set a working directory?
}



int main()
{
	const auto SONIA = parse_csv_resets_SONIA();

	const auto SONIA_compounded_index = parse_csv_resets_SONIA_compounded_index();

	auto detail = index_detail{};
	detail.initial_value = cpp_dec_float_50{ "100" };
	detail.step_rounding = 18u;
	detail.final_rounding = 8u;

	const auto date = 2025y / May / 13d;

	cout
		<< "For "
		<< date
		<< " SONIA Compounded Index is "
		<< SONIA_compounded_index[date] * 100 // ned a different accessor? (or handle 100 is some other way)
		<< " and the same computed value is "
		<< index(SONIA, date, detail);

	return 0;
}
