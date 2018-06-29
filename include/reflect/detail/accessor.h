// Copyright (c) 2018 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#ifndef REFLECT_DETAIL_ACCESSOR_H
#define REFLECT_DETAIL_ACCESSOR_H

//------------------------------------------------------------------------------
//--                     Begin Namespace Reflect::Detail                      --
namespace Reflect { namespace Detail {

// Uses.
template <typename T> class Buffer;
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

//----------------------------  Visitor Interface  -----------------------------
public:
    class Visitor {
    public:
        virtual void *visit(void *value, bool constant, bool temporary) = 0;
    };

    // Call visitor with a pointer to the value in storage, which must be of
    // the accessed type.
    virtual void *accept(Storage const &storage, Visitor &visitor) const = 0;

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
    // Retrieve the value in storage, which must be of the accessed type, as the
    // type associated with typeInfo.
    // An optional buffer may be provided into which an instance of the type
    // associated with typeInfo can be constructed (if necessary).
    // Throws an exception if the value cannot be retrieved as specified.
    void *getAs(Storage const &storage,
                TypeInfo const *typeInfo,
                Buffer<void> *buffer = nullptr) const;

    void const *getAsConst(Storage const &storage,
                           TypeInfo const *typeInfo,
                           Buffer<void> *buffer = nullptr) const;

    // Set the value in storage by copy-assigning the specified value.
    // Storage and value must both be of the accessed type.
    // Returns false if the accessed value cannot be assigned.
    virtual bool set(Storage &storage, void const *value) const {
        return false;
    }

    // Set the value in storage, which must be of the accessed type, by
    // copy-assigning the specified value of the type associated with typeInfo.
    // Throws an exception if the assignment cannot be made as specified.
    void setAs(Storage &storage,
               TypeInfo const *typeInfo,
               void const *value) const;

    void setAs(Storage &storage,
               Accessor const *accessor,
               Storage const &value) const;

    // Set the value in storage by move-assigning the specified value.
    // Storage and value must both be of the accessed type.
    // Defaults to copy-assignment if move-assignment is not possible.
    // Returns false if the accessed value cannot be assigned.
    virtual bool move(Storage &storage, void *value) const {
        return set(storage, value);
    }

    // Set the value in storage, which must be of the accessed type, by
    // move-assigning the specified value of the type associated with typeInfo.
    // Throws an exception if the assignment cannot be made as specified.
    void moveAs(Storage &storage,
                TypeInfo const *typeInfo,
                void *value) const;

    void moveAs(Storage &storage,
                Accessor const *accessor,
                Storage &value) const;

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
