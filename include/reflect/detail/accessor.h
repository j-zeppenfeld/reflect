// Copyright (c) 2018 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#ifndef REFLECT_DETAIL_ACCESSOR_H
#define REFLECT_DETAIL_ACCESSOR_H

//------------------------------------------------------------------------------
//--                     Begin Namespace Reflect::Detail                      --
namespace Reflect { namespace Detail {

// Uses.
class Storage;
class TypeInfo;

//------------------------------------------------------------------------------
//--                              Class Accessor                              --
//------------------------------------------------------------------------------
// Provides type-erased access to a storage.
class Accessor {
    // Not copyable nor assignable.
    Accessor(Accessor const &) = delete;
    Accessor &operator=(Accessor const &) = delete;

//-----------------------------  Public Interface  -----------------------------
public:
    // Retrieve the type information of the accessed type.
    TypeInfo const *getTypeInfo() const { return _typeInfo; }

//--------------------------------  Allocation  --------------------------------
public:
    // Allocate a copy of source within target.
    // Source and target must both be of the accessed type.
    // Returns an accessor for the allocated value in target.
    virtual Accessor const *allocateCopy(Storage const &source,
                                         Storage &target) const = 0;

    // Allocate a moved copy of source within target.
    // Source and target must both be of the accessed type.
    // Returns an accessor for the allocated value in target.
    virtual Accessor const *allocateMove(Storage &source,
                                         Storage &target) const {
        return allocateCopy(source, target);
    }

    // Allocate a reference to source within target.
    // Source and target must both be of the accessed type.
    // Returns an accessor for the allocated value in target.
    virtual Accessor const *allocateReference(Storage const &source,
                                              Storage &target,
                                              bool constant) const = 0;

    // Deallocate the value in storage, which must be of the accessed type.
    virtual void deallocate(Storage &storage) const = 0;

//-------------------------------  Value Access  -------------------------------
public:
    struct QualifiedValue {
        void *_value;
        bool _constant;

        QualifiedValue(void *value, bool constant)
        : _value(value), _constant(constant) { }
    };

    // Retrieve the value in storage, which must be of the accessed type.
    // An optional buffer may be provided into which an instance of the type
    // can be constructed using placement-new (if necessary).
    virtual QualifiedValue get(Storage const &storage,
                               void *buffer = nullptr) const = 0;

    // Set the value in storage by copy-assigning the specified value.
    // Storage and value must both be of the accessed type.
    // Returns false if the accessed value cannot be assigned.
    virtual bool set(Storage &storage, void const *value) const {
        return false;
    }

    // Set the value in storage by move-assigning the specified value.
    // Storage and value must both be of the accessed type.
    // Defaults to copy-assignment if move-assignment is not possible.
    // Returns false if the accessed value cannot be assigned.
    virtual bool move(Storage &storage, void *value) const {
        return set(storage, value);
    }

//----------------------------  Internal Interface  ----------------------------
protected:
    Accessor(TypeInfo const *typeInfo) : _typeInfo(typeInfo) { }
    virtual ~Accessor() { }

//-----------------------------  Private Members  ------------------------------
private:
    // Type information of the accessed type.
    TypeInfo const *_typeInfo;
};

} }
//--                      End Namespace Reflect::Detail                       --
//------------------------------------------------------------------------------

#endif
