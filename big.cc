#include "mtrace/mtrace.h"
#include "mtrace/malloc_counter.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include <boost/container/flat_set.hpp>

#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>
#include <map>
#include <set>
#include <vector>

using namespace boost::multi_index;

static const std::size_t ContainerSize = std::size_t(1e6);

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

    int get_x() const { return x; }

    int x;
    int y;
    std::unique_ptr<char[]> buffer;
};


struct B
{
    explicit B(int _x, int _y) :
      x(std::make_unique<int>(_x)), y(std::make_unique<int>(_y)), buffer(std::make_unique<char[]>(1024))
    {
    }

    ~B() =default;

    B(const B&) =delete;
    B& operator=(const B&) =delete;

    B(B&&) =default;
    B& operator=(B&&) =default;

    bool operator<(const B& rhs) const { return *x < *rhs.x; }

    int get_x() const { return *x; }

    std::unique_ptr<int> x;
    std::unique_ptr<int> y;
    std::unique_ptr<char[]> buffer;
};

template <typename Callable>
void run_benchmark(const std::string& desc, std::size_t iterations, Callable&& callable)
{
    auto start = std::chrono::steady_clock::now();

    for (std::size_t i = 0; i < iterations; ++i)
        callable();

    auto end = std::chrono::steady_clock::now();

    double total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    if (total_time < 1.0)
    {
        total_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << desc << ": total_time=" << total_time << "us";
    }
    else
    {
        std::cout << desc << ": total_time=" << total_time << "ms";
    }

    double per_iteration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / double(iterations);
    std::cout << " per_iteration=" << per_iteration << "ns" << std::endl;
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
    run_benchmark(desc + " <insert " + std::to_string(ContainerSize) + " elements>",
                  ContainerSize,
                  [&]()
                  {
                      c.emplace(rng(gen), rng(gen));
                  });

    volatile std::size_t x = 0;
    auto& view = c.template get<0>();

    run_benchmark(desc + " <lookup 100 elements>",
                  100,
                  [&]()
                  {
                      auto itt = view.find(rng(gen));
                      x += itt == view.cend();
                  });

    run_benchmark(desc + " <insert 100 elements>",
                  100,
                  [&]()
                  {
                      c.emplace(rng(gen), rng(gen));
                  });

    auto it = c.cbegin();
    run_benchmark(desc + " <container walk>",
                  c.size(),
                  [&]()
                  {
                      x += it->get_x();
                      ++it;
                  });

    malloc_counter& counter = mt.get<0>();
    std::cout << "malloc_calls=" << counter.malloc_calls() << " bytes_allocated=" << (counter.malloc_bytes() / std::size_t(1 << 20)) << "M" << std::endl;

    if (c.size() != ContainerSize + 100)
        throw std::runtime_error("unexpected container size");
}

template <typename T>
struct vector : public std::vector<T>
{
    template <typename... Args>
    void emplace(Args&&... args)
    {
        this->emplace_back(std::forward<Args>(args)...);
    }

    auto find(int i)
    {
        return std::lower_bound(this->cbegin(), this->cend(), T(i, i));
    }

    template <std::size_t N>
    auto& get()
    {
        auto start = std::chrono::steady_clock::now();
        std::sort(this->begin(), this->end());
        auto end = std::chrono::steady_clock::now();

        std::cout << "std::vector <sort>: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
        return *this;
    }
};

template <typename T>
struct flat_set : public boost::container::flat_set<T>
{
    template <std::size_t N>
    auto& get()
    {
        return *this;
    }

    auto find(int i)
    {
        return boost::container::flat_set<T>::find(A(i, i));
    }
};

template <typename T>
struct multiset : public std::multiset<T>
{
    template <std::size_t N>
    auto& get()
    {
        return *this;
    }

    auto find(int i)
    {
        return std::multiset<T>::find(A(i, i));
    }
};

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "usage: " << argv[0] << " <0..9>" << std::endl;
        return 1;
    }

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
        test_container<vector<A>>("std::vector<A>");
    else if (argv0 == "7")
        test_container<vector<B>>("std::vector<B>");
    else if (argv0 == "8")
        test_container<multiset<A>>("std::multiset");
    else if (argv0 == "9")
        test_container<flat_set<A>>("boost.flat_set");

    return 0;
}
