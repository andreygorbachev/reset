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

#include <variant>


namespace reset
{

	// separate these into separate files and maybe into a separate sub-library
	// if we need just 2 conventions we might want to just have 1 class with a flag

	template<typename T = double>
	class simple final
	{

	public:

		auto interest(
		) const noexcept -> T;

	};

	template<typename T = double>
	class compounded final
	{

	public:

		auto interest(
		) const noexcept -> T;

	};


	template<typename T = double>
	using rate = std::variant<
		simple<T>,
		compounded<T>
	>;


	template<typename T = double>
	auto interest(
		const rate<T>& r
	) -> T
	{
		const auto i = std::visit(
			[&](const auto& r) { return r.interest(); },
			r
		);

		return i;
	}

}
