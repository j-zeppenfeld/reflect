// Copyright (c) 2019 Johannes Zeppenfeld
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
    ValueAccessor() : Accessor(TypeInfo::instance<T>(), false, false) { }

    // Retrieve the global instance of this accessor.
    static Accessor const *instance() {
        static ValueAccessor accessor;
        return &accessor;
    }

//-------------------------------  Construction  -------------------------------
public:
    // Construct an instance of the accessed type within storage, forwarding
    // the provided arguments to the constructor.
    // Returns an accessor for the constructed value in storage.
    template <typename ...T_Args>
    static Accessor const *construct(Storage &storage, T_Args &&...args) {
        storage.construct<T>(std::forward<T_Args>(args)...);
        return instance();
    }

    // Construct a copy of value within storage.
    Accessor const *constructCopy(Storage &storage,
                                  Storage const &value) const override {
        storage.construct<T>(const_cast<T const &>(value.get<T>()));
        return this;
    }

    // Construct a moved copy of value within storage.
    Accessor const *constructMove(Storage &storage,
                                  Storage &value) const override {
        storage.construct<T>(std::move(value.get<T>()));
        return this;
    }

    // Construct a reference to value within storage.
    Accessor const *constructReference(Storage &storage,
                                       Storage const &value,
                                       bool constant) const override {
        if(constant) {
            return ValueAccessor<T const &>::construct(
                storage, const_cast<T const &>(value.get<T>())
            );
        } else {
            return ValueAccessor<T &>::construct(storage, value.get<T>());
        }
    }

    // Destruct the value in storage.
    void destruct(Storage &storage) const override {
        storage.destruct<T>();
    }

//----------------------------  Visitor Interface  -----------------------------
public:
    // Call visitor with a pointer to the value in storage.
    void *accept(Storage const &storage, Visitor &visitor) const override {
        return visitor.visit(&storage.get<T>(), false, false);
    }

//-------------------------------  Value Access  -------------------------------
public:
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
    ValueAccessor() : Accessor(TypeInfo::instance<T>(), false, true) { }

    // Retrieve the global instance of this accessor.
    static Accessor const *instance() {
        static ValueAccessor accessor;
        return &accessor;
    }

//-------------------------------  Construction  -------------------------------
public:
    // Construct a mutable reference to value within storage.
    // Returns an accessor for the constructed value in storage.
    static Accessor const *construct(Storage &storage, T &value) {
        storage.construct<T *>(&value);
        return instance();
    }

    // Construct a copy of value within storage.
    Accessor const *constructCopy(Storage &storage,
                                  Storage const &value) const override {
        return ValueAccessor<T>::construct(
            storage, const_cast<T const &>(*value.get<T *>())
        );
    }

    // Construct a moved copy of value within storage.
    Accessor const *constructMove(Storage &storage,
                                  Storage &value) const override {
        return ValueAccessor<T>::construct(
            storage, std::move(*value.get<T *>())
        );
    }

    // Construct a reference to value within storage.
    Accessor const *constructReference(Storage &storage,
                                       Storage const &value,
                                       bool constant) const override {
        if(constant) {
            return ValueAccessor<T const &>::construct(
                storage, const_cast<T const &>(*value.get<T *>())
            );
        } else {
            return construct(storage, *value.get<T *>());
        }
    }

    // Destruct the value in storage.
    void destruct(Storage &storage) const override {
        storage.destruct<T *>();
    }

//----------------------------  Visitor Interface  -----------------------------
public:
    // Call visitor with a pointer to the value in storage.
    void *accept(Storage const &storage, Visitor &visitor) const override {
        return visitor.visit(storage.get<T *>(), false, false);
    }

//-------------------------------  Value Access  -------------------------------
public:
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
    ValueAccessor() : Accessor(TypeInfo::instance<T>(), true, true) { }

    // Retrieve the global instance of this accessor.
    static Accessor const *instance() {
        static ValueAccessor accessor;
        return &accessor;
    }

//-------------------------------  Construction  -------------------------------
public:
    // Construct a constant reference to value within storage.
    // Returns an accessor for the constructed value in storage.
    static Accessor const *construct(Storage &storage, T const &value) {
        storage.construct<T const *>(&value);
        return instance();
    }

    // Construct a copy of value within storage.
    Accessor const *constructCopy(Storage &storage,
                                  Storage const &value) const override {
        return ValueAccessor<T>::construct(storage, *value.get<T const *>());
    }

    // Construct a reference to value within storage.
    Accessor const *constructReference(Storage &storage,
                                       Storage const &value,
                                       bool constant) const override {
        return construct(storage, *value.get<T const *>());
    }

    // Destruct the value in storage.
    void destruct(Storage &storage) const override {
        storage.destruct<T const *>();
    }

//----------------------------  Visitor Interface  -----------------------------
public:
    // Call visitor with a pointer to the value in storage.
    void *accept(Storage const &storage, Visitor &visitor) const override {
        return visitor.visit(const_cast<T *>(storage.get<T const *>()),
                             true, false);
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
    ValueAccessor() : Accessor(TypeInfo::instance<void>(), false, false) { }

    // Retrieve the global instance of this accessor.
    static Accessor const *instance() {
        static ValueAccessor accessor;
        return &accessor;
    }

//-------------------------------  Construction  -------------------------------
public:
    // Allocate a copy of source within target.
    Accessor const *constructCopy(Storage &storage,
                                  Storage const &value) const override {
        return this;
    }

    // Construct a reference to value within storage.
    Accessor const *constructReference(Storage &storage,
                                       Storage const &value,
                                       bool constant) const override {
        return this;
    }

    // Destruct the value in storage.
    void destruct(Storage &storage) const override { }

//----------------------------  Visitor Interface  -----------------------------
public:
    // Call visitor with a pointer to the value in storage.
    void *accept(Storage const &storage, Visitor &visitor) const override {
        return visitor.visit(nullptr, false, true);
    }

//-------------------------------  Value Access  -------------------------------
public:
    // Set the value in storage by copy-assigning the specified value.
    bool set(Storage &storage, void const *value) const override {
        return true;
    }
};

} }
//--                      End Namespace Reflect::Detail                       --
//------------------------------------------------------------------------------

#endif
