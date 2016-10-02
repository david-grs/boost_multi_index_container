#pragma once

#include <sstream>

struct counter
{
    counter() = default;
    virtual ~counter() {}

    counter(const counter&) { ++copy_ctor;; }
    counter& operator=(const counter&) { ++copy_assign; return *this; }

    counter(counter&&) { ++move_ctor; }
    counter& operator=(counter&&) { ++move_assign; return *this; }

    static void reset()
    {
        copy_ctor = 0;
        copy_assign = 0;
        move_ctor = 0;
        move_assign = 0;
    }

    static int copy_ctor;
    static int copy_assign;
    static int move_ctor;
    static int move_assign;
};

int counter::copy_ctor = 0;
int counter::copy_assign = 0;
int counter::move_ctor = 0;
int counter::move_assign = 0;

std::ostream& operator<<(std::ostream& os, const counter& a)
{
    return os << " copy_ctor=" << a.copy_ctor << " copy_assign=" << a.copy_assign <<
                         " move_ctor=" << a.move_ctor << " move_assign=" << a.move_assign;
}
