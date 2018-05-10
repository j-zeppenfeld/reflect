// Copyright (c) 2018 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#ifndef REFLECT_DETAIL_BASE_H
#define REFLECT_DETAIL_BASE_H

//------------------------------------------------------------------------------
//--                     Begin Namespace Reflect::Detail                      --
namespace Reflect { namespace Detail {

// Uses.
class TypeInfo;

//------------------------------------------------------------------------------
//--                                Class Base                                --
//------------------------------------------------------------------------------
// Contains information about a base class of a type.
class Base {
public:
    Base(TypeInfo const *typeInfo,
         void const *(*upcastFunc)(void const *value))
    : _typeInfo(typeInfo)
    , _upcastFunc(upcastFunc) { }

//-----------------------------  Public Interface  -----------------------------
public:
    // Retrieve the type information of the base type.
    TypeInfo const *getTypeInfo() const { return _typeInfo; }

    // Upcast value, which must be of the derived type, to the base type.
    void *upcast(void *value) const {
        return const_cast<void *>(_upcastFunc(value));
    }

    void const *upcast(void const *value) const {
        return _upcastFunc(value);
    }

//-----------------------------  Private Members  ------------------------------
private:
    // Type information of the base type.
    TypeInfo const *_typeInfo;
    // Pointer to upcast function.
    void const *(*_upcastFunc)(void const *value);
};

} }
//--                      End Namespace Reflect::Detail                       --
//------------------------------------------------------------------------------

#endif
