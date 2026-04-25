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

#include "decimal.h"

#include <ratio>
#include <string_view>


namespace reset
{

    template <typename Ratio>
    class scaled_value // should it be in util? // constexpr?
    {

    public:

        explicit scaled_value(std::string_view value); // noexcept?

        // note that we do this implicitly
        scaled_value(const Decimal& v); // noexcept?

    public:

        friend bool operator==(const scaled_value& x, const scaled_value& y) = default;
        friend bool operator<=>(const scaled_value& x, const scaled_value& y) = default;

        // note that we do this implicitly
        operator Decimal() const; // noexcept?

    public:

        auto get_value() const noexcept -> const Decimal&;

    private:

        Decimal value_;

    };


    template<typename Ratio>
    scaled_value<Ratio>::scaled_value(std::string_view value) :
        value_{ value }
    {
    }


    template<typename Ratio>
    scaled_value<Ratio>::scaled_value(const Decimal& v) :
        value_{ v * Ratio::den / Ratio::num }
    {
    }


    template<typename Ratio>
    scaled_value<Ratio>::operator Decimal() const
    {
        return value_ * Ratio::num / Ratio::den;
    }


    template<typename Ratio>
    auto scaled_value<Ratio>::get_value() const noexcept -> const Decimal&
    {
        return value_;
    }



    // literals? (not 100% sure what they would apply to as we are dealing with decimals, rather than doubles)
    // print format?


    using Percent = scaled_value<std::ratio<1, 100>>;
    
    using BasisPoints = scaled_value<std::ratio<1, 10'000>>;

    using Value = scaled_value<std::ratio<1, 1>>;

}
