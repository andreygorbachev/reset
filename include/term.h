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
#include <chrono>


namespace reset
{

    using term = std::variant<
        // I assume that business days would be handled elsewhere
        std::chrono::days,
        std::chrono::weeks,
        std::chrono::months,
        std::chrono::years
    >; // should this be in fin-calendar?


    template<typename... Ts>
    struct overloaded : Ts... { using Ts::operator()...; };


    [[nodiscard]] inline auto advance(std::chrono::year_month_day ymd, const term& trm) -> std::chrono::year_month_day
    {
        std::visit(overloaded{
            [&ymd](const std::chrono::days& ds) { ymd = std::chrono::sys_days{ ymd } + ds; },
            [&ymd](const std::chrono::weeks& ws) { ymd = std::chrono::sys_days{ ymd } + ws; },
            [&ymd](const std::chrono::months& ms) { ymd += ms; },
            [&ymd](const std::chrono::years& ys) { ymd += ys; },
            }, trm);

        // fix "bad" dates
        if (!ymd.ok())
            ymd = ymd.year() / ymd.month() / std::chrono::last;

        return ymd;
    }

    [[nodiscard]] inline auto retreat(std::chrono::year_month_day ymd, const term& trm) -> std::chrono::year_month_day
    {
        std::visit(overloaded{
            [&ymd](const std::chrono::days& ds) { ymd = std::chrono::sys_days{ ymd } - ds; },
            [&ymd](const std::chrono::weeks& ws) { ymd = std::chrono::sys_days{ ymd } - ws; },
            [&ymd](const std::chrono::months& ms) { ymd -= ms; },
            [&ymd](const std::chrono::years& ys) { ymd -= ys; },
            }, trm);

        // fix "bad" dates
        if (!ymd.ok())
            ymd = ymd.year() / ymd.month() / std::chrono::last;

        return ymd;
    }

}
