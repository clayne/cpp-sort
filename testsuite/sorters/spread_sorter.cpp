/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Morwenn
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <algorithm>
#include <ctime>
#include <iterator>
#include <random>
#include <string>
#include <vector>
#include <catch.hpp>
#include <cpp-sort/sorters/spread_sorter.h>
#include <cpp-sort/sort.h>

////////////////////////////////////////////////////////////
// static_if utility

template<bool Condition>
struct static_if
{
    template<typename Func>
    static void run(Func) {}
};

template<>
struct static_if<true>
{
    template<typename Func>
    static void run(Func func)
    {
        func();
    }
};

////////////////////////////////////////////////////////////
// Actual tests

TEST_CASE( "spread_sorter tests", "[spread_sorter]" )
{
    // Pseudo-random number engine
    std::mt19937_64 engine(std::time(nullptr));

    SECTION( "sort with int iterable" )
    {
        std::vector<int> vec(100'000);
        std::iota(std::begin(vec), std::end(vec), 0);
        std::shuffle(std::begin(vec), std::end(vec), engine);
        cppsort::sort(vec, cppsort::spread_sorter{});
        CHECK( std::is_sorted(std::begin(vec), std::end(vec)) );
    }

    SECTION( "sort with unsigned int iterators" )
    {
        std::vector<unsigned> vec(100'000);
        std::iota(std::begin(vec), std::end(vec), 0u);
        std::shuffle(std::begin(vec), std::end(vec), engine);
        cppsort::sort(std::begin(vec), std::end(vec), cppsort::spread_sorter{});
        CHECK( std::is_sorted(std::begin(vec), std::end(vec)) );
    }

    SECTION( "sort with float iterable" )
    {
        std::vector<float> vec(100'000);
        std::iota(std::begin(vec), std::end(vec), 0.0f);
        std::shuffle(std::begin(vec), std::end(vec), engine);
        cppsort::sort(vec, cppsort::spread_sorter{});
        CHECK( std::is_sorted(std::begin(vec), std::end(vec)) );
    }

    SECTION( "sort with double iterators" )
    {
        std::vector<double> vec(100'000);
        std::iota(std::begin(vec), std::end(vec), 0.0);
        std::shuffle(std::begin(vec), std::end(vec), engine);
        cppsort::sort(std::begin(vec), std::end(vec), cppsort::spread_sorter{});
        CHECK( std::is_sorted(std::begin(vec), std::end(vec)) );
    }

    SECTION( "sort with std::string iterable" )
    {
        std::vector<std::string> vec;
        for (int i = 0 ; i < 100'000 ; ++i)
        {
            vec.push_back(std::to_string(i));
        }
        std::shuffle(std::begin(vec), std::end(vec), engine);
        cppsort::sort(vec, cppsort::spread_sorter{});
        CHECK( std::is_sorted(std::begin(vec), std::end(vec)) );
    }

    SECTION( "sort with std::wstring iterators" )
    {
        static_if<sizeof(wchar_t) == 2>::run([&] {
            std::vector<std::wstring> vec;
            for (int i = 0 ; i < 100'000 ; ++i)
            {
                vec.push_back(std::to_wstring(i));
            }
            std::shuffle(std::begin(vec), std::end(vec), engine);
            cppsort::sort(std::begin(vec), std::end(vec), cppsort::spread_sorter{});
            CHECK( std::is_sorted(std::begin(vec), std::end(vec)) );
        });
    }

    SECTION( "reverse sort with std::string iterable" )
    {
        std::vector<std::string> vec;
        for (int i = 0 ; i < 100'000 ; ++i)
        {
            vec.push_back(std::to_string(i));
        }

        std::shuffle(std::begin(vec), std::end(vec), engine);
        cppsort::sort(vec, cppsort::spread_sorter{}, std::greater<>{});
        CHECK( std::is_sorted(std::begin(vec), std::end(vec), std::greater<>{}) );

        std::shuffle(std::begin(vec), std::end(vec), engine);
        cppsort::sort(vec, cppsort::spread_sorter{}, std::greater<std::string>{});
        CHECK( std::is_sorted(std::begin(vec), std::end(vec), std::greater<std::string>{}) );
    }

    SECTION( "reverse sort with std::wstring iterators" )
    {
        static_if<sizeof(wchar_t) == 2>::run([&] {
            std::vector<std::wstring> vec;
            for (int i = 0 ; i < 100'000 ; ++i)
            {
                vec.push_back(std::to_wstring(i));
            }

            std::shuffle(std::begin(vec), std::end(vec), engine);
            cppsort::sort(std::begin(vec), std::end(vec), cppsort::spread_sorter{}, std::greater<>{});
            CHECK( std::is_sorted(std::begin(vec), std::end(vec), std::greater<>{}) );

            std::shuffle(std::begin(vec), std::end(vec), engine);
            cppsort::sort(std::begin(vec), std::end(vec), cppsort::spread_sorter{}, std::greater<std::wstring>{});
            CHECK( std::is_sorted(std::begin(vec), std::end(vec), std::greater<std::wstring>{}) );
        });
    }
}
