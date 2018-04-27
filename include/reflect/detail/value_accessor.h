// Copyright (c) 2018 Johannes Zeppenfeld
// Distributed under the MIT license, see LICENSE file in repository root.

#ifndef REFLECT_DETAIL_VALUEACCESSOR_H
#define REFLECT_DETAIL_VALUEACCESSOR_H

#include "accessor.h"
#include "storage.h"

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
    // Retrieve the global instance of this accessor.
    static Accessor const *instance() {
        static ValueAccessor accessor;
        return &accessor;
    }

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
    // Retrieve the global instance of this accessor.
    static Accessor const *instance() {
        static ValueAccessor accessor;
        return &accessor;
    }

    // Allocate a copy of source within target.
    Accessor const *allocateCopy(Storage const &source,
                                 Storage &target) const override {
        target.construct<T>(*source.get<T *>());
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
    // Retrieve the global instance of this accessor.
    static Accessor const *instance() {
        static ValueAccessor accessor;
        return &accessor;
    }

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
};

//------------------------------------------------------------------------------
//--                        Class ValueAccessor<void>                         --
//------------------------------------------------------------------------------
// Dummy accessor for void values.
template <>
class ValueAccessor<void> : public Accessor {
public:
    // Retrieve the global instance of this accessor.
    static Accessor const *instance() {
        static ValueAccessor accessor;
        return &accessor;
    }

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
};

} }
//--                      End Namespace Reflect::Detail                       --
//------------------------------------------------------------------------------

#endif
