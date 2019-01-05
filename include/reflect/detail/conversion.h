// Copyright (c) 2019 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#ifndef REFLECT_DETAIL_CONVERSION_H
#define REFLECT_DETAIL_CONVERSION_H

//------------------------------------------------------------------------------
//--                     Begin Namespace Reflect::Detail                      --
namespace Reflect { namespace Detail {

// Uses.
class Accessor;
template <typename T> class Buffer;
class Storage;
class TypeInfo;

//------------------------------------------------------------------------------
//--                             Class Conversion                             --
//------------------------------------------------------------------------------
// Contains information about a conversion from one type to another.
class Conversion {
public:
    Conversion(TypeInfo const *typeInfo,
               void *(*getFunc)(void const *value, Buffer<void> &buffer),
               bool (*setFunc)(Accessor const *accessor, Storage &storage,
                               void const *value),
               bool (*moveFunc)(Accessor const *accessor, Storage &storage,
                                void *value))
    : _typeInfo(typeInfo)
    , _getFunc(getFunc)
    , _setFunc(setFunc)
    , _moveFunc(moveFunc) { }

//-----------------------------  Public Interface  -----------------------------
public:
    // Retrieve the type information of the target type.
    TypeInfo const *getTypeInfo() const { return _typeInfo; }

    // Retrieve value, which must be of the source type, as the target type.
    // A buffer must be provided into which an instance of the target type can
    // be constructed (if necessary).
    void *get(void const *value, Buffer<void> &buffer) const {
        return _getFunc(value, buffer);
    }

    // Set the value in storage, which must be of the accessed type, by
    // copy-assigning value, which must be of the source type.
    bool set(Accessor const *accessor, Storage &storage,
             void const *value) const {
        return _setFunc(accessor, storage, value);
    }

    // Set the value in storage, which must be of the accessed type, by
    // move-assigning value, which must be of the source type.
    bool move(Accessor const *accessor, Storage &storage,
              void *value) const {
        return _moveFunc(accessor, storage, value);
    }

//-----------------------------  Private Members  ------------------------------
private:
    // Type information of the target type.
    TypeInfo const *_typeInfo;
    // Pointer to conversion function.
    void *(*_getFunc)(void const *value, Buffer<void> &buffer);
    // Pointer to converting set function.
    bool (*_setFunc)(Accessor const *accessor, Storage &storage,
                     void const *value);
    // Pointer to converting move function.
    bool (*_moveFunc)(Accessor const *accessor, Storage &storage,
                      void *value);
};

} }
//--                      End Namespace Reflect::Detail                       --
//------------------------------------------------------------------------------

#endif
