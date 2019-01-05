// Copyright (c) 2019 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#ifndef REFLECT_TYPE_H
#define REFLECT_TYPE_H

#include "detail/iterator_range.h"
#include "detail/iterator_value.h"
#include "detail/type_info.h"

#include <ostream>

//------------------------------------------------------------------------------
//--                         Begin Namespace Reflect                          --
namespace Reflect {

//------------------------------------------------------------------------------
//--                                Class Type                                --
//------------------------------------------------------------------------------
// This class provides information about a reflected type.
class Type {
//-----------------------------  Public Interface  -----------------------------
public:
    // Retrieve the shortest name by which the type has been registered.
    std::string const &getName() const { return _typeInfo->getName(); }

    // Returns true if the type is constant, e.g., T const.
    bool isConstant() const { return _constant; }

    // Returns true if the type is a reference, e.g., T &.
    bool isReference() const { return _reference; }

//-------------------------------  Base Classes  -------------------------------
public:
    // Iterate over all registered base class types of the type.
    using BaseIterator = Detail::IteratorValue<
        Detail::TypeInfo::BaseIterator, Type
    >;
    Detail::IteratorRange<BaseIterator> getBases() const {
        return { beginBases(), endBases() };
    }
    BaseIterator beginBases() const {
        return _typeInfo->beginBases();
    }
    BaseIterator endBases() const {
        return _typeInfo->endBases();
    }

//-------------------------------  Conversions  --------------------------------
public:
    // Iterate over all registered conversion target types of the type.
    using ConversionIterator = Detail::IteratorValue<
        Detail::TypeInfo::ConversionIterator, Type
    >;
    Detail::IteratorRange<ConversionIterator> getConversions() const {
        return { beginConversions(), endConversions() };
    }
    ConversionIterator beginConversions() const {
        return _typeInfo->beginConversions();
    }
    ConversionIterator endConversions() const {
        return _typeInfo->endConversions();
    }

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
    // Internal general-purpose constructor, use the templated getType function
    // instead.
    Type(Detail::TypeInfo const *typeInfo, bool constant, bool reference)
    : _typeInfo(typeInfo), _constant(constant), _reference(reference) { }

    // Internal constructor for base class type iteration, use the derived
    // type's base class type iteration methods instead.
    Type(Detail::Base const &base)
    : _typeInfo(base.getTypeInfo())
    , _constant(false)
    , _reference(false) { }

    // Internal constructor for conversion target type iteration, use the source
    // type's conversion target type iteration methods instead.
    Type(Detail::Conversion const &conversion)
    : _typeInfo(conversion.getTypeInfo())
    , _constant(false)
    , _reference(false) { }

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
