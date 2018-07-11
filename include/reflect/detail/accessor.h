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

    // Returns true if the accessed type is constant.
    bool isConstant() const { return _constant; }

    // Returns true if the accessed type is a reference.
    bool isReference() const { return _reference; }

//----------------------------  Visitor Interface  -----------------------------
public:
    class Visitor {
    public:
        virtual void *visit(void *value, bool constant, bool temporary) = 0;
    };

    // Call visitor with a pointer to the value in storage, which must be of
    // the accessed type.
    virtual void *accept(Storage const &storage, Visitor &visitor) const = 0;

//-------------------------------  Construction  -------------------------------
public:
    // Construct a copy of value within storage.
    // Value must be of the accessed type, and storage must be unallocated.
    // Returns an accessor for the constructed value in storage.
    virtual Accessor const *constructCopy(Storage &storage,
                                          Storage const &value) const = 0;

    // Construct a moved copy of value within storage.
    // Value must be of the accessed type, and storage must be unallocated.
    // Returns an accessor for the constructed value in storage.
    virtual Accessor const *constructMove(Storage &storage,
                                          Storage &value) const {
        return constructCopy(storage, value);
    }

    // Construct a reference to value within storage.
    // Value must be of the accessed type, and storage must be unallocated.
    // Returns an accessor for the constructed value in storage.
    virtual Accessor const *constructReference(Storage &storage,
                                               Storage const &value,
                                               bool constant) const = 0;

    // Destruct the value in storage, which must be of the accessed type.
    virtual void destruct(Storage &storage) const = 0;

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
    Accessor(TypeInfo const *typeInfo, bool constant, bool reference)
    : _typeInfo(typeInfo), _constant(constant), _reference(reference) { }
    virtual ~Accessor() { }

//-----------------------------  Private Members  ------------------------------
private:
    // Type information of the accessed type.
    TypeInfo const *_typeInfo;
    // Constant qualifier of the accessed type.
    bool _constant;
    // Reference qualifier of the accessed type.
    bool _reference;
};

} }
//--                      End Namespace Reflect::Detail                       --
//------------------------------------------------------------------------------

#endif
