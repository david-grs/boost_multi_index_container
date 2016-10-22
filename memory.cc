#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>

#include <iostream>
#include <unordered_set>

using namespace boost::multi_index;

template <typename T>
struct tracker
{
    tracker()
    {
        s_objects.insert(this);
    }
    tracker(const tracker&)
    {
        s_objects.insert(this);
    }
    tracker(tracker&&)
    {
        s_objects.insert(this);
    }

    ~tracker()
    {
        int deleted = s_objects.erase(this);
        assert(deleted);
    }

    static void print_instances()
    {
        std::cout << s_objects.size() << " instances:" << std::endl;
        for (auto&& obj : s_objects)
            std::cout << "  " << obj << std::endl;
    }

private:
    static std::unordered_set<void*> s_objects;
};

template <typename T>
std::unordered_set<void*> tracker<T>::s_objects;

struct key : tracker<key>
{
    explicit key(int k) : _k(k) {}
    bool operator==(const key& rhs) const { return _k == rhs._k; }
    int _k;
};

struct value : tracker<value>
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
    {
        key k(1);
        tracker<key>::print_instances();
    }
    key k1(1);
    key k2(k1);
    {
        key k3(1);
        tracker<key>::print_instances();
    }
    tracker<key>::print_instances();

}
