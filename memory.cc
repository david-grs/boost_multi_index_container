#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/sequenced_index.hpp>

#include <boost/container/static_vector.hpp>

#include <iostream>
#include <unordered_set>

using namespace boost::multi_index;

boost::container::static_vector<std::pair<void*, int>, 256> sv;

void* operator new(std::size_t n)
{
    void* p = malloc(n);
    sv.emplace_back(p, n);
//    std::cout << "allocating " << n << " bytes at " << p << std::endl;
    return p;
}
void* operator new[](std::size_t n)
{
    void* p = malloc(n);
    sv.emplace_back(p, n);
   // std::cout << "allocating " << n << " bytes at " << p << std::endl;
    return p;
}
void operator delete(void* p)
{
  //  std::cout << "deleting " << p << std::endl;
    auto it = std::find_if(sv.begin(), sv.end(), [&](auto&& pair) { return pair.first == p; });
    sv.erase(it);
    return free(p);
}
void operator delete(void* p, std::size_t n)
{
  //  std::cout << "deleting " << n << " bytes at " << p << std::endl;
    auto it = std::find_if(sv.begin(), sv.end(), [&](auto&& pair) { return pair.first == p && pair.second == n; });
    sv.erase(it);
    return free(p);
}
void operator delete[](void* p)
{
  //  std::cout << "deleting " << p << std::endl;
    auto it = std::find_if(sv.begin(), sv.end(), [&](auto&& pair) { return pair.first == p; });
    sv.erase(it);
    return free(p);
}
void operator delete[](void* p, std::size_t n)
{
    //std::cout << "deleting " << n << " bytes at " << p << std::endl;
    auto it = std::find_if(sv.begin(), sv.end(), [&](auto&& pair) { return pair.first == p && pair.second == n; });
    sv.erase(it);
    return free(p);
}

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
        (void)deleted;
    }

    static void print_instances()
    {
        std::cout << s_objects.size() << " " << typeid(T).name() << " instances:" << std::endl;
        for (auto&& obj : s_objects)
            std::cout << "  " << obj << std::endl;
    }

private:
    static std::unordered_set<void*> s_objects;
};

template <typename T>
std::unordered_set<void*> tracker<T>::s_objects;

template <typename Tag>
struct key : tracker<key<Tag>>
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
    template <typename Tag>
    struct hash<key<Tag>>
    {
        size_t operator()(const key<Tag>& rhs) const { return hash<int>()(rhs._k); }
    };
}

namespace tags
{
    struct k1 {};
    struct k2 {};
}

struct A : tracker<A>
{
    A(int i, int j) :
     k(i),
     k2(i),
     v(j), _i(i)
    {}

    key<tags::k1> k;
    key<tags::k2> k2;
    value v;
    int _i;
};

int main()
{
    boost::multi_index_container<
      A,
      indexed_by<
        hashed_unique<
          member<A, key<tags::k1>, &A::k>,
          std::hash<key<tags::k1>>
        >,
        hashed_non_unique<
          member<A, key<tags::k2>, &A::k2>,
          std::hash<key<tags::k2>>
        >,
        ordered_unique<
          member<A, int, &A::_i>
        >,
        sequenced<>
      >
    > m;

    A a(1, 2);
    m.insert(A(1,2));
    m.insert(A(2,2));
    m.insert(A(3,2));
    m.insert(A(4,2));
    m.insert(A(5,2));


    sv.clear();
    std::cout << " start " << std::endl;
    m.insert(A(0x00f00ba3, 0x00f00ba3));
    m.insert(A(0xdeadbeef, 0xdeadbeef));
    std::cout << " stop" << std::endl;

    for (auto&& s: sv)
    {
        std::cout << s.first << " " << s.second << std::endl;

        int64_t* p = (int64_t*)s.first;
        for (; p < s.first + s.second; ++p)
            std::cout << "    " << (void*)*p;
        std::cout << std::endl;
    }

    tracker<key<tags::k1>>::print_instances();
    tracker<key<tags::k2>>::print_instances();
    tracker<value>::print_instances();
    tracker<A>::print_instances();
    return 0;
}
