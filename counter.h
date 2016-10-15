#pragma once

#include <sstream>
#include <type_traits>
#include <typeinfo>

template <typename T, typename Tag = T>
struct counter
{
    template <typename... Args, typename = typename std::enable_if<std::is_constructible<T, Args...>::value>::type>
    counter(Args&&... args) : _t(std::forward<Args>(args)...) { ++ctor; }

    virtual ~counter() { ++dtor; }

    counter(const counter& c) { _t = c._t; ++copy_ctor; }
    counter& operator=(const counter& c) { _t = c._t; ++copy_assign; return *this; }

    counter(counter&& c) : _t(std::move(c._t)) { ++move_ctor; }
    counter& operator=(counter&& c) {  _t = std::move(c._t); ++move_assign; return *this; }

    const T& get() const { return _t; }
    T& get() { return _t; }

    bool operator==(const counter<T, Tag>& c) const { return _t == c._t; }

    static void reset()
    {
        ctor = 0;
        dtor = 0;
        copy_ctor = 0;
        copy_assign = 0;
        move_ctor = 0;
        move_assign = 0;
    }

    static int ctor;
    static int dtor;
    static int copy_ctor;
    static int copy_assign;
    static int move_ctor;
    static int move_assign;

private:
    T _t;
};

template <typename T, typename Tag>
int counter<T, Tag>::ctor = 0;

template <typename T, typename Tag>
int counter<T, Tag>::dtor = 0;

template <typename T, typename Tag>
int counter<T, Tag>::copy_ctor = 0;

template <typename T, typename Tag>
int counter<T, Tag>::copy_assign = 0;

template <typename T, typename Tag>
int counter<T, Tag>::move_ctor = 0;

template <typename T, typename Tag>
int counter<T, Tag>::move_assign = 0;

template <typename T, typename Tag>
std::ostream& operator<<(std::ostream& os, const counter<T, Tag>& a)
{
    return os << typeid(T).name() << typeid(Tag).name() << " ctor=" << a.ctor << " dtor=" << a.dtor << " copy_ctor=" << a.copy_ctor
                << " copy_assign=" << a.copy_assign << " move_ctor=" << a.move_ctor << " move_assign=" << a.move_assign;
}


namespace std
{

template <typename T, typename Tag>
struct hash<counter<T, Tag>>
{
    std::size_t operator()(const counter<T, Tag>& c) const
    {
        return typename std::hash<T>()(c.get());
    }
};

}
