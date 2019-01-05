// Copyright (c) 2019 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#ifndef REFLECT_TESTS_COMMON_CLASSES_H
#define REFLECT_TESTS_COMMON_CLASSES_H

#include "count.h"

//------------------------------------------------------------------------------
//--                                Class Base                                --
//------------------------------------------------------------------------------
class Base : private Count<Base> {
public:
    Base() = default;

    Base(Base const &other)
    : Count(other)
    , _from(&other)
    , _i(other._i)
    , _s(other._s) { }

    Base(Base &&other)
    : Count(std::move(other))
    , _from(&other)
    , _i(other._i)
    , _s(std::move(other._s)) { }

    Base(int i)
    : Count(i)
    , _i(i) { }

    Base(std::string s)
    : Count(s)
    , _s(s) { }

    Base(int i, std::string s)
    : Count(i, s)
    , _i(i)
    , _s(std::move(s)) { }

    Base &operator=(Base const &other) {
        Count::operator=(other);
        _from = &other;
        return *this;
    }

    Base &operator=(Base &&other) {
        Count::operator=(std::move(other));
        _from = &other;
        return *this;
    }

    Base &operator=(int i) {
        Count::operator=(i);
        _i = i;
        return *this;
    }

    Base &operator=(std::string s) {
        Count::operator=(s);
        _s = std::move(s);
        return *this;
    }

    Base const *getFrom() const {
        return _from;
    }

    int getInt() const {
        return _i;
    }

    std::string const &getString() const {
        return _s;
    }

private:
    Base const *_from = nullptr;
    int _i = -1;
    std::string _s = "<not set>";
};

//------------------------------------------------------------------------------
//--                              Class Derived                               --
//------------------------------------------------------------------------------
class Derived : public Base, private Count<Derived> {
public:
    Derived() = default;
    Derived(Derived const &) = default;
    Derived(Derived &&) = default;
    Derived(int i) : Base(i), Count<Derived>(i) { }
    Derived(std::string s) : Base(s), Count<Derived>(s) { }
    Derived(int i, std::string s) : Base(i, s), Count<Derived>(i, s) { }

    Derived &operator=(Derived const &) = default;
    Derived &operator=(Derived &&) = default;
    Derived &operator=(int i) {
        Base::operator=(i);
        Count<Derived>::operator=(i);
        return *this;
    }
    Derived &operator=(std::string s) {
        Base::operator=(s);
        Count<Derived>::operator=(s);
        return *this;
    }
};

//------------------------------------------------------------------------------
//--                             Class Unrelated                              --
//------------------------------------------------------------------------------
class Unrelated : private Count<Unrelated> {
public:
    Unrelated() = default;
    Unrelated(Unrelated const &) = default;
    Unrelated(Unrelated &&) = default;

    Unrelated &operator=(Unrelated const &) = default;
    Unrelated &operator=(Unrelated &&) = default;
};

#endif
