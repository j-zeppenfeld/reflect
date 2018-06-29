// Copyright (c) 2018 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#ifndef REFLECT_DETAIL_BUFFER_H
#define REFLECT_DETAIL_BUFFER_H

// std::forward
#include <utility>

//------------------------------------------------------------------------------
//--                     Begin Namespace Reflect::Detail                      --
namespace Reflect { namespace Detail {

//------------------------------------------------------------------------------
//--                               Class Buffer                               --
//------------------------------------------------------------------------------
// Provides a properly sized and aligned memory region into which an instance
// of a certain type can be constructed.
template <typename T = void> class Buffer;

template <>
class Buffer<void> {
public:
    // Construct the instance of the buffer, passing the specified arguments.
    // Requires that the buffer has not already been constructed, and that type
    // T is equivalent to the type of the buffer.
    template <typename T, typename ...T_Args>
    T *construct(T_Args &&...args) {
        T *value = new(_buffer) T(std::forward<T_Args>(args)...);
        _constructed = true;
        return value;
    }

    // Construct the instance of the buffer by copying value.
    // Requires that the buffer has not already been constructed, and that value
    // is of the same type as the buffer.
    virtual void *constructCopy(void const *value) = 0;

    // Construct the instance of the buffer by moving value.
    // Requires that the buffer has not already been constructed, and that value
    // is of the same type as the buffer.
    virtual void *constructMove(void *value) {
        return constructCopy(value);
    }

    // Returns true if the buffer has been constructed.
    bool isConstructed() const { return _constructed; }

protected:
    Buffer(void *buffer)
    : _buffer(buffer)
    , _constructed(false) { }

    virtual ~Buffer() = default;

private:
    void *_buffer;
    bool _constructed;
};

template <typename T>
class Buffer : public Buffer<void> {
public:
    Buffer() : Buffer<void>(&_value) { }
    ~Buffer() { if(isConstructed()) _value.~T(); }

    // Construct the instance of the buffer, passing the specified arguments.
    // Requires that the buffer has not already been constructed.
    template <typename ...T_Args>
    T *construct(T_Args &&...args) {
        return Buffer<void>::construct<T>(std::forward<T_Args>(args)...);
    }

    // Construct the instance of the buffer by copying value.
    void *constructCopy(void const *value) override {
        return construct(*static_cast<T const *>(value));
    }

    // Construct the instance of the buffer by moving value.
    void *constructMove(void *value) override {
        return construct(std::move(*static_cast<T *>(value)));
    }

    // Retrieve the buffer's constructed value.
    // Requires that the buffer has been constructed.
    T &getValue() {
        return _value;
    }

//-----------------------------  Private Members  ------------------------------
private:
    union { T _value; };
};

} }
//--                      End Namespace Reflect::Detail                       --
//------------------------------------------------------------------------------

#endif
