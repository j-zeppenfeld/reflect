// Copyright (c) 2018 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#ifndef REFLECT_TYPE_H
#define REFLECT_TYPE_H

#include <ostream>

//------------------------------------------------------------------------------
//--                         Begin Namespace Reflect                          --
namespace Reflect {

// Uses.
namespace Detail { class TypeInfo; }

//------------------------------------------------------------------------------
//--                                Class Type                                --
//------------------------------------------------------------------------------
// This class provides information about a reflected type.
class Type {
//-----------------------------  Public Interface  -----------------------------
public:
    // Returns true if the type is constant, e.g., T const.
    bool isConstant() const { return _constant; }

    // Returns true if the type is a reference, e.g., T &.
    bool isReference() const { return _reference; }

//--------------------------------  Operators  ---------------------------------
public:
    // Comparison operators providing an ordering of types.
    // The order is consistent throughout a single invocation of the program,
    // but may change from one invocation to the next.
    friend bool operator==(Type const &lhs, Type const &rhs);
    friend bool operator!=(Type const &lhs, Type const &rhs);
    friend bool operator<(Type const &lhs, Type const &rhs);
    friend bool operator<=(Type const &lhs, Type const &rhs);
    friend bool operator>(Type const &lhs, Type const &rhs);
    friend bool operator>=(Type const &lhs, Type const &rhs);

    // Output streaming operator.
    friend std::ostream &operator<<(std::ostream &os, Type const &type);

//----------------------------  Internal Interface  ----------------------------
public:
    // Internal constructor, use the templated getType function instead.
    Type(Detail::TypeInfo const *typeInfo, bool constant, bool reference)
    : _typeInfo(typeInfo), _constant(constant), _reference(reference) { }

//-----------------------------  Private Members  ------------------------------
private:
    // Type information associated with the type.
    Detail::TypeInfo const *_typeInfo;
    // Constant qualifier.
    bool _constant;
    // Reference qualifier.
    bool _reference;
};

//---------------------------  Non-Member Functions  ---------------------------

// Create a type instance for type T.
template <typename T>
Type getType();

}
//--                          End Namespace Reflect                           --
//------------------------------------------------------------------------------

#include "type.hpp"

#endif
