#pragma once

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>

#include <string>

namespace impl
{

using namespace boost::multi_index;

struct stock
{
    std::string market_ref; // exchange specific
    std::string id;         // unique company-wide
    double price;
    int volume;
};

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

}

using impl::stock;
using impl::message_handler;

