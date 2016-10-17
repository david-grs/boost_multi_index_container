#include "counter.h"

#include <string>
#include <map>
#include <experimental/string_view>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/move/move.hpp>

#include <iostream>
#include <unordered_map>

using namespace boost::multi_index;

namespace tags {

struct first_name {};
struct last_name {};

}

using first_name_t = counter<std::string, tags::first_name>;
using last_name_t = counter<std::string, tags::last_name>;

struct employee
{
    std::string full_name() const { return first_name.get() + " " + last_name.get(); }

    std::experimental::string_view first_name_view() const { return {first_name.get().c_str(), first_name.get().size()}; }

    first_name_t first_name;
    last_name_t last_name;
    int age;
    double salary;
};


struct by_name{};

void simple_index()
{
    boost::multi_index_container<
      employee,
      indexed_by<
        hashed_unique<
          tag<by_name>,
          BOOST_MULTI_INDEX_CONST_MEM_FUN(employee, std::experimental::string_view, first_name_view),
          std::hash<std::experimental::string_view>
        >
      >
    > employees;

    auto&& v = employees.get<by_name>();

    first_name_t::reset();
    auto p = v.insert({"john", "doe", 21, 2000.0});
    std::cout << "boost.mic insert " << p.first->first_name << std::endl;

    const char* buff = "john";
    std::experimental::string_view vbuff{buff, std::strlen(buff)};

    first_name_t::reset();

    auto it = v.find(vbuff);
    std::cout << it->first_name << std::endl;
}

void copy_key_umap()
{
    std::unordered_map<first_name_t, employee> m;

    first_name_t::reset();
    auto p = m.emplace("john", employee{"john", "doe", 21, 2000.0});
    std::cout << "umap insert " << p.first->second.first_name << std::endl;
}

int main()
{
    simple_index();
    copy_key_umap();
    return 0;
}
