// Copyright (c) 2018 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#ifndef REFLECT_DETAIL_ITERATORVALUE_H
#define REFLECT_DETAIL_ITERATORVALUE_H

//------------------------------------------------------------------------------
//--                     Begin Namespace Reflect::Detail                      --
namespace Reflect { namespace Detail {

//------------------------------------------------------------------------------
//--                           Class IteratorValue                            --
//------------------------------------------------------------------------------
// Iterator adaptor that makes the underlying iterator of type T_It appear to
// iterate over values of type T_Value.
// Requires that T_Value can be constructed from the value type of T_It.
template <typename T_It, typename T_Value>
class IteratorValue {
    // Private proxy class used to return a pointer to a temporary value.
    class Proxy;

public:
    // Iterator-defining types.
    using value_type = T_Value;
    using pointer = T_Value *;
    using reference = T_Value &;
    using difference_type = typename T_It::difference_type;
    using iterator_category = typename T_It::iterator_category;

public:
    // Default construct with an uninitialized underlying iterator.
    IteratorValue() = default;

    // Construct iterator with the specified underlying iterator.
    IteratorValue(T_It it)
    : _it(it) { }

//--------------------------------  Operators  ---------------------------------
public:
    // Access operators.
    T_Value operator*() const {
        return *_it;
    }
    Proxy operator->() const {
        return _it;
    }

    // Iteration operators.
    IteratorValue &operator++() { ++_it; return *this; }
    IteratorValue operator++(int) { return _it++; }

    IteratorValue &operator--() { --_it; return *this; }
    IteratorValue operator--(int) { return _it++; }

    // Comparison operators.
    friend bool operator==(IteratorValue const &lhs, IteratorValue const &rhs) {
        return (lhs._it == rhs._it);
    }
    friend bool operator!=(IteratorValue const &lhs, IteratorValue const &rhs) {
        return (lhs._it != rhs._it);
    }

//----------------------------  Private Interface  -----------------------------
private:
    // Proxy class used to return a pointer to a temporary value for operator->.
    class Proxy {
    public:
        Proxy(T_It it) : _value(*it) { }
        T_Value *operator->() { return &_value; }
    private:
        T_Value _value;
    };

//-----------------------------  Private Members  ------------------------------
private:
    // Underlying iterator.
    T_It _it;
};

} }
//--                      End Namespace Reflect::Detail                       --
//------------------------------------------------------------------------------

#endif
