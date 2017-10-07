#include "mtrace/mtrace.h"
#include "mtrace/malloc_counter.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>
#include <map>
#include <set>
#include <vector>

using namespace boost::multi_index;

struct A
{
    A(int _x, int _y) :
      x(_x), y(_y), buffer(std::make_unique<char[]>(1024))
    {
    }

    ~A() =default;

    A(const A&) =delete;
    A& operator=(const A&) =delete;

    A(A&&) =default;
    A& operator=(A&&) =default;

    bool operator<(const A& rhs) const { return x < rhs.x; }

    int x;
    int y;
    std::unique_ptr<char[]> buffer;
};

template <typename Callable>
void run_benchmark(const std::string& desc, Callable&& callable)
{
    static const int Iterations = 1e6;

    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < Iterations; ++i)
        callable(i);
    auto end = std::chrono::steady_clock::now();

    std::cout << desc << ": " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
};

template <typename ContainerT>
void test_container(const std::string& desc)
{
    std::random_device rd;
    auto seed = rd();

    std::mt19937 gen(seed);
    std::uniform_int_distribution<> rng(0, 1e6);

    mtrace<malloc_counter> mt;

    ContainerT c;
    run_benchmark(desc + " <insert>",
                  [&](int i)
                  {
                      c.emplace(rng(gen), rng(gen));
                  });

    malloc_counter& counter = mt.get<0>();
    std::cout << "malloc_calls=" << counter.malloc_calls() << " bytes_allocated=" << (counter.malloc_bytes() / std::size_t(1 << 20)) << "M" << std::endl;
}

template <typename T>
struct vector : public std::vector<T>
{
    vector() { this->reserve(int(1e6)); }
    template <typename... Args>
    void emplace(Args&&... args) { this->emplace_back(std::forward<Args>(args)...); }
};

int main(int argc, char** argv)
{
    using MIC1Index = boost::multi_index_container<
        A,
        indexed_by<
        ordered_non_unique<
            member<A, int, &A::x>
        >
        >
    >;

    using MIC2Indexes = boost::multi_index_container<
        A,
        indexed_by<
        ordered_non_unique<
            member<A, int, &A::x>
        >,
        ordered_non_unique<
            member<A, int, &A::y>
        >
        >
    >;

    using MIC4Indexes = boost::multi_index_container<
        A,
        indexed_by<
        ordered_non_unique<
            member<A, int, &A::x>
        >,
        ordered_non_unique<
            member<A, int, &A::y>
        >,
        ordered_non_unique<
            member<A, int, &A::x>,
            std::greater<int>
        >,
        ordered_non_unique<
            member<A, int, &A::y>,
            std::greater<int>
        >
        >
    >;

    using MIC8Indexes = boost::multi_index_container<
        A,
        indexed_by<
        ordered_non_unique<
            member<A, int, &A::x>
        >,
        ordered_non_unique<
            member<A, int, &A::y>
        >,
        ordered_non_unique<
            member<A, int, &A::x>,
            std::greater<int>
        >,
        ordered_non_unique<
            member<A, int, &A::y>,
            std::greater<int>
        >,
        ordered_non_unique<
            member<A, int, &A::x>
        >,
        ordered_non_unique<
            member<A, int, &A::y>
        >,
        ordered_non_unique<
            member<A, int, &A::x>,
            std::greater<int>
        >,
        ordered_non_unique<
            member<A, int, &A::y>,
            std::greater<int>
        >
        >
    >;

    using MIC16Indexes = boost::multi_index_container<
        A,
        indexed_by<
        ordered_non_unique<
            member<A, int, &A::x>
        >,
        ordered_non_unique<
            member<A, int, &A::y>
        >,
        ordered_non_unique<
            member<A, int, &A::x>,
            std::greater<int>
        >,
        ordered_non_unique<
            member<A, int, &A::y>,
            std::greater<int>
        >,
        ordered_non_unique<
            member<A, int, &A::x>
        >,
        ordered_non_unique<
            member<A, int, &A::y>
        >,
        ordered_non_unique<
            member<A, int, &A::x>,
            std::greater<int>
        >,
        ordered_non_unique<
            member<A, int, &A::y>,
            std::greater<int>
        >,
        ordered_non_unique<
            member<A, int, &A::x>
        >,
        ordered_non_unique<
            member<A, int, &A::y>
        >,
        ordered_non_unique<
            member<A, int, &A::x>,
            std::greater<int>
        >,
        ordered_non_unique<
            member<A, int, &A::y>,
            std::greater<int>
        >,
        ordered_non_unique<
            member<A, int, &A::x>
        >,
        ordered_non_unique<
            member<A, int, &A::y>
        >,
        ordered_non_unique<
            member<A, int, &A::x>,
            std::greater<int>
        >,
        ordered_non_unique<
            member<A, int, &A::y>,
            std::greater<int>
        >
        >
    >;

    const std::string argv0(argv[1]);
    if (argv0 == "1")
        test_container<MIC1Index>("boost::mic 1 index");
    else if (argv0 == "2")
        test_container<MIC2Indexes>("boost::mic 2 indexes");
    else if (argv0 == "3")
        test_container<MIC4Indexes>("boost::mic 4 indexes");
    else if (argv0 == "4")
        test_container<MIC8Indexes>("boost::mic 8 indexes");
    else if (argv0 == "5")
        test_container<MIC16Indexes>("boost::mic 16 indexes");
    else if (argv0 == "6")
        test_container<vector<A>>("std::vector");
    else if (argv0 == "7")
        test_container<std::multiset<A>>("std::set");

#if 0
    volatile int x = 0; // its only reason is to avoid the compiler to optimize lookups

    {
        benchmark([&]() { mic.emplace(rng(gen), rng(gen)); }, "boost.mic insert");

        auto&& h = mic.get<tags::unordered>();
        benchmark([&]() { x += h.find(A(rng(gen), rng(gen))) != h.end(); }, "boost.mic lookup");

        auto&& asc = mic.get<tags::x_asc>();
        auto it = asc.begin();
        benchmark([&]()
        {
            if (it == asc.end())
                it = asc.begin();
            x += it->x;
            ++it;
        }, "boost.mic walk");
        benchmark([&]() { h.erase(A(rng(gen), rng(gen))); }, "boost.mic erase");
    }
#endif

    return 0;
}
