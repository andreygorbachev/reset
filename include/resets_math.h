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


namespace reset
{

	template<typename T>
	constexpr auto round_dp(const T x, const unsigned int decimal_places) -> T
	{
		const auto p = pow(T{ 10 }, decimal_places);
		return round(x * p) / p;
	}

	template<typename T>
	constexpr auto trunc_dp(const T x, const unsigned int decimal_places) -> T
	{
		const auto p = pow(T{ 10 }, decimal_places);
		return trunc(x * p) / p;
	}


	constexpr auto from_percent(const auto val)
	{
		return val / 100;
	}

	constexpr auto to_percent(const auto val)
	{
		return val * 100;
	}

}
