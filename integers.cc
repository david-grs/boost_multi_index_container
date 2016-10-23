#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>
#include <set>

namespace tags {
struct asc {};
struct desc {};
}

using namespace boost::multi_index;

int main()
{
    auto benchmark = [](auto&& operation, const char* desc)
    {
        static const int Iterations = 1e6;

        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < Iterations; ++i)
            operation();
        auto end = std::chrono::steady_clock::now();

        std::cout << desc << ": " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
    };


    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> rng(0, 1e6);
    volatile int found = 0; // its only reason is to avoid the compiler to optimize all the lookups

    {
        boost::multi_index_container<
          int,
          indexed_by<
            ordered_unique<
              tag<tags::asc>,
              identity<int>,
              std::less<int>
            >,
            ordered_unique<
              tag<tags::desc>,
              identity<int>,
              std::greater<int>
            >
          >
        > mic;

        benchmark([&]() { mic.insert(rng(gen)); }, "boost.mic insert");
        benchmark([&]() { found += mic.find(rng(gen)) != mic.end(); }, "boost.mic lookup");
        benchmark([&]() { mic.erase(rng(gen)); }, "boost.mic erase");
    }

    {
        std::set<int> asc;
        std::set<int> desc;

        benchmark([&]()
        {
            int n = rng(gen);
            asc.insert(n);
            desc.insert(n);
        }, "std::set insert");

        benchmark([&]() { found += asc.find(rng(gen)) != asc.end(); }, "std::set lookup");
        benchmark([&]()
        {
            int n = rng(gen);
            asc.erase(n);
            desc.erase(n);
        }, "std::set erase");
    }


#if 0
    auto&& v = m.get<tags::desc>();

    for (auto&& i : v)
        std::cout << i << std::endl;
#endif
}
