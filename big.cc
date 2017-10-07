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
    explicit A(int _x, int _y) :
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

    volatile int x = 0;
    auto& view = c.template get<0>();

    run_benchmark(desc + " <lookup>",
                  [&](int i)
                  {
                      auto it = view.find(rng(gen));
                      x += it == view.cend();
                  });

    malloc_counter& counter = mt.get<0>();
    std::cout << "malloc_calls=" << counter.malloc_calls() << " bytes_allocated=" << (counter.malloc_bytes() / std::size_t(1 << 20)) << "M" << std::endl;
}

// ugly hack
template <typename T>
struct vector : public std::vector<T>
{
    template <typename... Args>
    void emplace(Args&&... args) { this->emplace_back(std::forward<Args>(args)...); }

    auto find(int i) { return std::lower_bound(this->cbegin(), this->cend(), T(i, i)); }

    template <std::size_t N>
    auto& get()
    {
        // ugly hack^2
        std::sort(this->begin(), this->end());
        return *this;
    }
};

// ugly hack^3
template <typename T>
struct multiset : public std::multiset<T>
{
    template <std::size_t N>
    auto& get() { return *this; }

    auto find(int i) { return std::multiset<T>::find(A(i, i)); }
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
        test_container<multiset<A>>("std::multiset");

    return 0;
}
