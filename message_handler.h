#pragma once

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>

#include <string>
#include <unordered_map>
#include <experimental/string_view>

namespace impl
{

using namespace boost::multi_index;

struct stock
{
    stock(const std::string& _market_ref, const std::string& _id, double _price, int _volume) :
        market_ref(_market_ref),
        market_ref_view(market_ref.data(), market_ref.size()),
        id(_id),
        price(_price),
        volume(_volume)
    {}

    std::string market_ref; // exchange specific
    std::experimental::string_view market_ref_view;
    std::string id;         // unique company-wide
    double price;
    int volume;
};

struct market_data_provider
{
    void add_stock(stock&& s)
    {
        m_stocks.insert(s);
        m_stocks2.emplace(s.market_ref, std::move(s));
    }

    void on_price_change_mic(const char* market_ref, double new_price)
    {
        auto& view = m_stocks.get<by_reference>();
        
        auto it = view.find(market_ref);

        if (it == view.end())
            throw std::runtime_error("stock " + std::string(market_ref) + " not found");

        const_cast<stock&>(*it).price = new_price; // fine, price is not an index
    }

    void on_price_change_mic_view(const char* market_ref, int len, double new_price)
    {
        auto& view = m_stocks.get<by_reference_view>();

        std::experimental::string_view ref_view(market_ref, len);
        auto it = view.find(ref_view);

        if (it == view.end())
            throw std::runtime_error("stock " + std::string(market_ref) + " not found");

        const_cast<stock&>(*it).price = new_price; // fine, price is not an index
    }

    void on_price_change_umap(const char* market_ref, double new_price)
    {
        auto it = m_stocks2.find(market_ref);

        if (it == m_stocks2.end())
            throw std::runtime_error("stock " + std::string(market_ref) + " not found");

        it->second.price = new_price;
    }

private:

    struct by_reference {};
    struct by_reference_view {};

    boost::multi_index_container<
      stock,
      indexed_by<
        hashed_unique<
          tag<by_reference>,
          BOOST_MULTI_INDEX_MEMBER(stock, std::string, market_ref)
        >,
        hashed_unique<
          tag<by_reference_view>,
          BOOST_MULTI_INDEX_MEMBER(stock, std::experimental::string_view, market_ref_view),
          std::hash<std::experimental::string_view>
        >
      >
    > m_stocks;

    std::unordered_map<std::string, stock> m_stocks2;
};

}

using impl::stock;
using impl::market_data_provider;

