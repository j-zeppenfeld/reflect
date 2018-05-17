// Copyright (c) 2018 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#ifndef REFLECT_DETAIL_VALUEACCESSOR_H
#define REFLECT_DETAIL_VALUEACCESSOR_H

#include "accessor.h"
#include "storage.h"
#include "type_info.h"

// std::decay, std::is_same et al.
#include <type_traits>
// std::move
#include <utility>

//------------------------------------------------------------------------------
//--                     Begin Namespace Reflect::Detail                      --
namespace Reflect { namespace Detail {

//------------------------------------------------------------------------------
//--                          Class ValueAccessor<T>                          --
//------------------------------------------------------------------------------
// Provides access to a mutable value in storage.
template <typename T>
class ValueAccessor : public Accessor {
    static_assert(std::is_same<T, typename std::decay<T>::type>::value,
                  "Internal error: Accessor instance must be decomposed.");

public:
    // Default constructible.
    ValueAccessor() : Accessor(TypeInfo::instance<T>()) { }

    // Retrieve the global instance of this accessor.
    static Accessor const *instance() {
        static ValueAccessor accessor;
        return &accessor;
    }

//--------------------------------  Allocation  --------------------------------
public:
    // Allocate a copy of source within target.
    Accessor const *allocateCopy(Storage const &source,
                                 Storage &target) const override {
        target.construct<T>(source.get<T>());
        return this;
    }

    // Allocate a moved copy of source within target.
    Accessor const *allocateMove(Storage &source,
                                 Storage &target) const override {
        target.construct<T>(std::move(source.get<T>()));
        return this;
    }

    // Allocate a reference to source within target.
    Accessor const *allocateReference(Storage const &source,
                                      Storage &target,
                                      bool constant) const override {
        if(constant) {
            target.construct<T const *>(&source.get<T>());
            return ValueAccessor<T const &>::instance();
        } else {
            target.construct<T *>(&source.get<T>());
            return ValueAccessor<T &>::instance();
        }
    }

    // Deallocate the value in storage.
    void deallocate(Storage &storage) const override {
        storage.destruct<T>();
    }

//-------------------------------  Value Access  -------------------------------
public:
    // Retrieve the value in storage, which must be of the accessed type.
    QualifiedValue get(Storage const &storage, void *buffer) const override {
        return { &storage.get<T>(), false };
    }

    // Set the value in storage by copy-assigning the specified value.
    bool set(Storage &storage, void const *value) const override {
        storage.get<T>() = *static_cast<T const *>(value);
        return true;
    }

    // Set the value in storage by move-assigning the specified value.
    bool move(Storage &storage, void *value) const override {
        storage.get<T>() = std::move(*static_cast<T *>(value));
        return true;
    }
};

//------------------------------------------------------------------------------
//--                         Class ValueAccessor<T &>                         --
//------------------------------------------------------------------------------
// Provides access to a mutable reference in storage.
template <typename T>
class ValueAccessor<T &> : public Accessor {
    static_assert(std::is_same<T, typename std::decay<T>::type>::value,
                  "Internal error: Accessor instance must be decomposed.");

public:
    // Default constructible.
    ValueAccessor() : Accessor(TypeInfo::instance<T>()) { }

    // Retrieve the global instance of this accessor.
    static Accessor const *instance() {
        static ValueAccessor accessor;
        return &accessor;
    }

//--------------------------------  Allocation  --------------------------------
public:
    // Allocate a copy of source within target.
    Accessor const *allocateCopy(Storage const &source,
                                 Storage &target) const override {
        target.construct<T>(*source.get<T *>());
        return ValueAccessor<T>::instance();
    }

    // Allocate a moved copy of source within target.
    Accessor const *allocateMove(Storage &source,
                                 Storage &target) const override {
        target.construct<T>(std::move(*source.get<T *>()));
        return ValueAccessor<T>::instance();
    }

    // Allocate a reference to source within target.
    Accessor const *allocateReference(Storage const &source,
                                      Storage &target,
                                      bool constant) const override {
        if(constant) {
            target.construct<T const *>(source.get<T *>());
            return ValueAccessor<T const &>::instance();
        } else {
            target.construct<T *>(source.get<T *>());
            return this;
        }
    }

    // Deallocate the value in storage.
    void deallocate(Storage &storage) const override {
        storage.destruct<T *>();
    }

//-------------------------------  Value Access  -------------------------------
public:
    // Retrieve the value in storage, which must be of the accessed type.
    QualifiedValue get(Storage const &storage, void *buffer) const override {
        return { storage.get<T *>(), false };
    }

    // Set the value in storage by copy-assigning the specified value.
    bool set(Storage &storage, void const *value) const override {
        *storage.get<T *>() = *static_cast<T const *>(value);
        return true;
    }

    // Set the value in storage by move-assigning the specified value.
    bool move(Storage &storage, void *value) const override {
        *storage.get<T *>() = std::move(*static_cast<T *>(value));
        return true;
    }
};

//------------------------------------------------------------------------------
//--                      Class ValueAccessor<T const &>                      --
//------------------------------------------------------------------------------
// Provides access to a constant reference in storage.
template <typename T>
class ValueAccessor<T const &> : public Accessor {
    static_assert(std::is_same<T, typename std::decay<T>::type>::value,
                  "Internal error: Accessor instance must be decomposed.");

public:
    // Default constructible.
    ValueAccessor() : Accessor(TypeInfo::instance<T>()) { }

    // Retrieve the global instance of this accessor.
    static Accessor const *instance() {
        static ValueAccessor accessor;
        return &accessor;
    }

//--------------------------------  Allocation  --------------------------------
public:
    // Allocate a copy of source within target.
    Accessor const *allocateCopy(Storage const &source,
                                 Storage &target) const override {
        target.construct<T>(*source.get<T const *>());
        return ValueAccessor<T>::instance();
    }

    // Allocate a reference to source within target.
    Accessor const *allocateReference(Storage const &source,
                                      Storage &target,
                                      bool constant) const override {
        target.construct<T const *>(source.get<T const *>());
        return this;
    }

    // Deallocate the value in storage.
    void deallocate(Storage &storage) const override {
        storage.destruct<T const *>();
    }

//-------------------------------  Value Access  -------------------------------
public:
    // Retrieve the value in storage, which must be of the accessed type.
    QualifiedValue get(Storage const &storage, void *buffer) const override {
        return { const_cast<T *>(storage.get<T const *>()), true };
    }
};

//------------------------------------------------------------------------------
//--                        Class ValueAccessor<void>                         --
//------------------------------------------------------------------------------
// Dummy accessor for void values.
template <>
class ValueAccessor<void> : public Accessor {
public:
    // Default constructible.
    ValueAccessor() : Accessor(TypeInfo::instance<void>()) { }

    // Retrieve the global instance of this accessor.
    static Accessor const *instance() {
        static ValueAccessor accessor;
        return &accessor;
    }

//--------------------------------  Allocation  --------------------------------
public:
    // Allocate a copy of source within target.
    Accessor const *allocateCopy(Storage const &source,
                                 Storage &target) const override {
        return this;
    }

    // Allocate a reference to source within target.
    Accessor const *allocateReference(Storage const &source,
                                      Storage &target,
                                      bool constant) const override {
        return this;
    }

    // Deallocate the value in storage.
    void deallocate(Storage &storage) const override { }

//-------------------------------  Value Access  -------------------------------
public:
    // Retrieve the value in storage, which must be of the accessed type.
    QualifiedValue get(Storage const &storage, void *buffer) const override {
        return { nullptr, true };
    }
};

} }
//--                      End Namespace Reflect::Detail                       --
//------------------------------------------------------------------------------

#endif
