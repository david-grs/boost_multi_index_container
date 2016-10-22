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
    explicit value(int v) : _v(v) {}
    int _v;
};


namespace std {
    template <>
    struct hash<key>
    {
        size_t operator()(const key& rhs) const { return hash<int>()(rhs._k); }
    };
}

struct A
{
    A(int i, int j) :
     k(i),
     v(j)
    {}

    key k;
    value v;
};

int main()
{
    boost::multi_index_container<
      A,
      indexed_by<
        hashed_unique<
          BOOST_MULTI_INDEX_MEMBER(A, key, k),
          std::hash<key>
        >
      >
    > m;

    m.insert(A(1, 2));
    m.insert(A(2, 3));
    m.insert(A(3, 4));

    tracker<key>::print_instances();
    tracker<value>::print_instances();
    return 0;
}
