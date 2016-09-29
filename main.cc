
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>

#include <string>

struct stock
{
    std::string market_ref; // exchange specific
    std::string id;         // unique company-wide
    double price;
    int volume;
};

namespace
{
    using namespace boost::multi_index;
}

struct message_handler
{

private:

    struct by_reference {};

    boost::multi_index_container<
      stock,
      indexed_by<
        hashed_unique<
          tag<by_reference>,
          BOOST_MULTI_INDEX_MEMBER(stock, std::string, market_ref)
        >
      >
    > mEvents;
};

int main()
{
  return 0;
}
