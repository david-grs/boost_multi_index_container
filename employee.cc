#include <string>
#include <map>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>

#include <iostream>

using namespace boost::multi_index;

struct employee
{
  std::string first_name;
  std::string last_name;
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
            BOOST_MULTI_INDEX_MEMBER(employee, std::string, first_name)
        >
      >
    > employees;

    auto&& v = employees.get<by_name>();
    v.insert({"john", "doe", 21, 2000.0});

    for (auto&& e : v)
      std::cout << e.first_name << std::endl;
}

void composed_index()
{
    boost::multi_index_container<
      employee,
      indexed_by<
        hashed_unique<
          tag<by_name>,
          composite_key<
            employee,
            BOOST_MULTI_INDEX_MEMBER(employee, std::string, first_name),
            BOOST_MULTI_INDEX_MEMBER(employee, std::string, last_name)
          >
        >
      >
    > employees;

    auto&& v = employees.get<by_name>();
    v.insert({"john", "doe", 21, 2000.0});

    auto it = v.find(boost::make_tuple("john", "doe"));
    std::cout << it->first_name << std::endl;
}


int main()
{
    simple_index();
    composed_index();

    return 0;
}
