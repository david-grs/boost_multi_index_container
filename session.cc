#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>

#include <string>
#include <chrono>
#include <map>
#include <iostream>
#include <vector>
#include <algorithm>
#include <experimental/string_view>

using namespace boost::multi_index;

struct session
{
    session() =default;

    session(const std::string& user, const std::string& script) :
        user_name(user),
        script_name(script)
    {}

    std::string id() const { return user_name.substr(0, 3) + script_name.substr(0, 3); }

    std::experimental::string_view user_name_view() const { return user_name; }
    std::experimental::string_view script_name_view() const { return script_name; }
    std::string user_name;
    std::string script_name;

    std::chrono::time_point<std::chrono::system_clock> started_time;
};

struct by_name{};

void simple_index()
{
    boost::multi_index_container<
      session,
      indexed_by<
        hashed_unique<
          tag<by_name>,
          member<session, std::string, &session::user_name>
        >
      >
    > sessions;

    auto&& v = sessions.get<by_name>();
    v.insert({"john", "foo.py"});

    for (auto&& e : v)
      std::cout << e.user_name << std::endl;

    auto it = v.find("john");
    std::cout << it->user_name << std::endl;
}

void composed_index()
{
    boost::multi_index_container<
        session,
        indexed_by<
        hashed_unique<
            composite_key<
            session,
            const_mem_fun<session, std::experimental::string_view, &session::user_name_view>,
            const_mem_fun<session, std::experimental::string_view, &session::script_name_view>
            >,
            composite_key_hash<
            std::hash<std::experimental::string_view>,
            std::hash<std::experimental::string_view>
            >
          >
        >
    > sessions;

    sessions.insert({"john", "foo.py"});

    auto it = sessions.find(boost::make_tuple("john", "foo.py"));
    std::cout << it->user_name << std::endl;
}

void function_index()
{
    boost::multi_index_container<
      session,
      indexed_by<
        hashed_unique<
          tag<by_name>,
          const_mem_fun<session, std::string, &session::id>
        >
      >
    > sessions;

    auto&& v = sessions.get<by_name>();
    v.insert({"john", "foo.py"});

    auto it = v.find("johfoo");
    std::cout << it->user_name << std::endl;
}

void map_multiple_index()
{
    using full_name = std::pair<std::string, std::string>;
    std::map<full_name, session> m;
    m[{"john", "foo.py"}] = session{};

    for (auto&& p : m)
      std::cout << p.first.first << " " << p.first.second << std::endl;

    auto it = m.find(std::make_pair("john", "foo.py"));
    std::cout << it->second.user_name << std::endl;
}

void bla()
{
    struct session
    {
        std::string user_name;
        std::string script_name;
    };


    boost::multi_index_container<
      session,
      indexed_by<
        hashed_unique<
          tag<by_name>,
          composite_key<
            session,
              member<session, std::string, &session::user_name>,
              member<session, std::string, &session::script_name>
          >
        >
      >
    > sessions;
}

int main()
{
    map_multiple_index();
    simple_index();
    composed_index();
    function_index();

    return 0;
}
