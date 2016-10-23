#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include <algorithm>
#include <iostream>
#include <random>

namespace tags {
struct asc {};
struct desc {};
}

using namespace boost::multi_index;

int main()
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
    > m;

    std::random_device rd; //container
    std::mt19937 gen(rd()); //ruleset for rd(merzenne twister)
    std::uniform_int_distribution<> rng;


    auto&& v = m.get<tags::desc>();

    for (auto&& i : v)
        std::cout << i << std::endl;

}
