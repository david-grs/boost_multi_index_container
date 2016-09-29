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

struct market_data_provider
{
    void add_stock(stock&& s)
    {
        m_stocks.insert(std::move(s));
    }

    void on_price_change(const char* market_ref, double new_price)
    {
        auto& view = m_stocks.get<by_reference>();
        auto it = view.find(market_ref);

        if (it == view.end())
            throw std::runtime_error("stock " + std::string(market_ref) + " not found");

        const_cast<stock&>(*it).price = new_price; // fine, price is not an index
    };

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
    > m_stocks;
};

}

using impl::stock;
using impl::market_data_provider;

