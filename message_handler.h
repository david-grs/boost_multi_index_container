#pragma once

#include "counter.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>

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
        market_ref_view(_market_ref.data(), _market_ref.size()),
        id(_id),
        price(_price),
        volume(_volume)
    {}

    stock(const stock&) = default;
    stock(stock&&) = default;
    stock& operator=(const stock&) = default;
    stock& operator=(stock&&) = default;

    const char* get_market_ref() const { return market_ref.get().c_str(); }

    counter<std::string> market_ref; // exchange specific
    counter<std::experimental::string_view> market_ref_view;
    std::string id;         // unique company-wide
    double price;
    int volume;
};

struct market_data_provider_mic_string
{
    static const char* name() { return "boost::mic<string>"; }

    void add_stock(const stock& s)
    {
        m_stocks.insert(s);
    }

    void on_price_change(const char* market_ref, int /*len*/, double new_price)
    {
        auto& view = m_stocks.get<by_reference>();

        // using this temp std::string cut by half the number of std::string copies in boost.mic
        counter<std::string> str_market_ref(market_ref);
        auto it = view.find(str_market_ref);

        if (it == view.end())
            throw std::runtime_error("stock " + std::string(market_ref) + " not found");

        const_cast<stock&>(*it).price = new_price; // fine, price is not an index
    }

private:
    struct by_reference {};

    boost::multi_index_container<
      stock,
      indexed_by<
        hashed_unique<
          tag<by_reference>,
          member<stock, counter<std::string>, &stock::market_ref>,
          std::hash<counter<std::string>>
        >
      >
    > m_stocks;
};


struct market_data_provider_mic_string_view
{
    static const char* name() { return "boost::mic<string_view>"; }

    void add_stock(const stock& s)
    {
        m_stocks.insert(s);
    }

    void on_price_change(const char* market_ref, int len, double new_price)
    {
        auto& view = m_stocks.get<by_reference_view>();

        counter<std::experimental::string_view> ref_view(market_ref, len);
        auto it = view.find(ref_view);

        if (it == view.end())
            throw std::runtime_error("stock " + std::string(market_ref) + " not found");

        const_cast<stock&>(*it).price = new_price; // fine, price is not an index
    }

private:
    struct by_reference_view {};

    boost::multi_index_container<
      stock,
      indexed_by<
        hashed_unique<
          tag<by_reference_view>,
          member<stock, counter<std::experimental::string_view>, &stock::market_ref_view>,
          std::hash<counter<std::experimental::string_view>>
        >
      >
    > m_stocks;
};


struct market_data_provider_umap_string
{
    static const char* name() { return "unordered_map<string>"; }

    void add_stock(const stock& s)
    {
        m_stocks.emplace(s.market_ref.get(), s);
    }

    void on_price_change(const char* market_ref, int /*len*/, double new_price)
    {
        auto it = m_stocks.find(market_ref);

        if (it == m_stocks.end())
            throw std::runtime_error("stock " + std::string(market_ref) + " not found");

        it->second.price = new_price;
    }

private:
    std::unordered_map<std::string, stock> m_stocks;
};


struct market_data_provider_umap_string_view
{
    static const char* name() { return "unordered_map<string_view>"; }

    void add_stock(const stock& s)
    {
        m_stocks.insert({{s.market_ref.get().c_str(), s.market_ref.get().size()}, s});
    }

    void on_price_change(const char* market_ref, int len, double new_price)
    {
        std::experimental::string_view ref_view(market_ref, len);
        auto it = m_stocks.find(ref_view);

        if (it == m_stocks.end())
            throw std::runtime_error("stock " + std::string(market_ref) + " not found");

        it->second.price = new_price;
    }

private:
    std::unordered_map<std::experimental::string_view, stock> m_stocks;
};

}

using impl::stock;
using impl::market_data_provider_mic_string;
using impl::market_data_provider_mic_string_view;
using impl::market_data_provider_umap_string;
using impl::market_data_provider_umap_string_view;

