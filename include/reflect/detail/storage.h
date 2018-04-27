// Copyright (c) 2018 Johannes Zeppenfeld
// Distributed under the MIT license, see LICENSE file in repository root.

#ifndef REFLECT_DETAIL_STORAGE_H
#define REFLECT_DETAIL_STORAGE_H

// std::aligned_storage et al.
#include <type_traits>
// std::forward
#include <utility>

//------------------------------------------------------------------------------
//--                     Begin Namespace Reflect::Detail                      --
namespace Reflect { namespace Detail {

//------------------------------------------------------------------------------
//--                              Class Storage                               --
//------------------------------------------------------------------------------
class Storage {
//-----------------------------  Public Interface  -----------------------------
public:
    // Construct an instance of type T within the storage, forwarding the
    // provided arguments to the constructor.
    // Requires that the storage was previously unallocated.
    template <typename T, typename ...T_Args>
    T &construct(T_Args &&...args) {
        return *new(allocate<T>()) T{std::forward<T_Args>(args)...};
    }

    // Destruct the previously constructed instance of type T from within the
    // storage.
    // Requires that the storage was previously allocated and constructed with
    // type T.
    template <typename T>
    void destruct() {
        access<T>()->~T();
        deallocate<T>();
    }

    // Retrieve the previously allocated instance from the storage.
    // Requires that the storage was previously allocated and constructed with
    // type T.
    template <typename T>
    T &get() const {
        return *access<T>();
    }

//----------------------------  Internal Interface  ----------------------------
private:
    template <typename T>
    using Buffer = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

    // Allocate storage for placement new of an instance of type T.
    // Requires that the storage was previously unallocated.
    template <typename T>
    void *allocate() {
        static_assert(std::is_same<T, typename std::decay<T>::type>::value,
                      "Internal error: storage type must be decayed.");

        // TODO: Small buffer optimization.
        _data = new Buffer<T>;
        return _data;
    }

    // Deallocate storage for placement new of an instance of type T.
    // Requires that the storage was previously allocated, and that any
    // instances of type T constructed therein have been destructed.
    template <typename T>
    void deallocate() {
        static_assert(std::is_same<T, typename std::decay<T>::type>::value,
                      "Internal error: storage type must be decayed.");

        // TODO: Small buffer optimization.
        delete static_cast<Buffer<T> *>(_data);
    }

    // Retrieve a pointer to the instance of type T held by the storage.
    // Requires that the storage was previously allocated and constructed with
    // type T.
    template <typename T>
    T *access() const {
        static_assert(std::is_same<T, typename std::decay<T>::type>::value,
                      "Internal error: storage type must be decayed.");

        // TODO: Small buffer optimization.
        return static_cast<T *>(_data);
    }

//-----------------------------  Private Members  ------------------------------
private:
    void *_data;
};

} }
//--                      End Namespace Reflect::Detail                       --
//------------------------------------------------------------------------------

#endif
