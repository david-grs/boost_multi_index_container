#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>

#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>
#include <set>
#include <unordered_set>

namespace tags {
struct x_asc {};
struct y_asc {};
struct unordered {};
}

struct A
{
    A(int _x, int _y) :
      x(_x), y(_y)
    {}

    bool operator==(const A& a) const { return x == a.x && y == a.y; }

    int x;
    int y;
};

struct CompX
{
    bool operator()(const A& lhs, const A& rhs) const { return lhs.x < rhs.x; }
};

struct CompY
{
    bool operator()(const A& lhs, const A& rhs) const { return lhs.y < rhs.y; }
};

namespace std
{

template <>
struct hash<A>
{
    std::size_t operator()(const A& a) const
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, a.x);
        boost::hash_combine(seed, a.y);
        return seed;
    }
};

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
    auto seed = rd();

    std::mt19937 gen(seed);
    std::uniform_int_distribution<> rng(0, 1e6);
    volatile int x = 0; // its only reason is to avoid the compiler to optimize lookups

    {
        boost::multi_index_container<
          A,
          indexed_by<
            ordered_unique<
              tag<tags::x_asc>,
              member<A, int, &A::x>,
              std::less<int>
            >,
            ordered_unique<
              tag<tags::y_asc>,
              member<A, int, &A::y>,
              std::greater<int>
            >,
            hashed_unique<
              tag<tags::unordered>,
              identity<A>,
              std::hash<A>
            >
          >
        > mic;

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

    // we use the same range of integers for the 2 tests
    gen.seed(seed);

    {
        std::set<A, CompX> asc;
        std::set<A, CompY> desc;
        std::unordered_set<A> h;

        benchmark([&]()
        {
            A a{rng(gen), rng(gen)};
            asc.insert(a);
            desc.insert(a);
            h.insert(a);
        }, "std::sets insert");

        benchmark([&]() { x += h.find(A(rng(gen), rng(gen))) != h.end(); }, "std::containers lookup");
        auto it = asc.begin();
        benchmark([&]()
        {
            if (it == asc.end())
                it = asc.begin();
            x += it->x;
            ++it;
        }, "std::set walk");
        benchmark([&]()
        {
            A a{rng(gen), rng(gen)};
            asc.erase(a);
            desc.erase(a);
            h.erase(a);
        }, "std::containers erase");
    }

    return 0;
}
