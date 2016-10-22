#include "counter.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>

#include <string>
#include <map>
#include <experimental/string_view>
#include <iostream>
#include <unordered_map>

using namespace boost::multi_index;

struct key
{
    explicit key(int k) : _k(k) {}
    bool operator==(const key& rhs) const { return _k == rhs._k; }
    int _k;
};

struct value
{
    explicit value(int k) : _k(k) {}
    bool operator==(const value& rhs) const { return _k == rhs._k; }
    int _k;
};

namespace std {
    template <>
    struct hash<key>
    {
        size_t operator()(const key& rhs) { return hash<int>()(rhs._k); }
    };
    template <>
    struct hash<value>
    {
        size_t operator()(const value& rhs) { return hash<int>()(rhs._k); }
    };
}

int main()
{
}
