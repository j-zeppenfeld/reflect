// Copyright (c) 2019 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#ifndef REFLECT_OBJECT_H
#define REFLECT_OBJECT_H

#include "detail/storage.h"
#include "detail/traits.h"

// std::reference_wrapper
#include <functional>

//------------------------------------------------------------------------------
//--                         Begin Namespace Reflect                          --
namespace Reflect {

// Uses.
namespace Detail { class Accessor; }
namespace Detail { class Property; }
class Type;

//------------------------------------------------------------------------------
//--                               Class Object                               --
//------------------------------------------------------------------------------
// This class provides reflection access to a contained value of type T or a
// type derived therefrom. If T is void, any type can be contained. The actual
// type of the contained value is called the reflected type of the object, and
// can differ from type T (so long as the reflected type derives from T or T is
// void).
// The contained value may be owned by the object, or merely be referenced by
// the object.
template <typename T = void>
class Object {
public:
    static_assert(std::is_same<T, typename std::decay<T>::type>::value,
                  "Object must be of unqualified type.");

    using element_type = T;

public:
    // Construct object containing a copy of other.
    // The reflected type of the object will be T_Derived.
    template <
        typename T_Derived,
        Detail::EnableIf<
            Detail::IsDerived<T_Derived, T>::value &&
            std::is_constructible<T_Derived, T_Derived>::value &&
            !Detail::IsReflected<T_Derived>::value &&
            !Detail::IsSameTemplate<T_Derived, std::reference_wrapper<T>>::value
        > = Detail::EnableIfType::Enabled
    >
    Object(T_Derived &&other);

    // Construct object containing a copy of the other object's value.
    // The reflected type of the object will be equivalent to that of other.
    Object(Object<T> const &other);

    // Construct object containing the other object's moved value.
    // The reflected type of the object will be equivalent to that of other.
    Object(Object<T> &&other);

    // Construct object containing a copy of the other object's value.
    // The reflected type of the object will be equivalent to that of other.
    // Throws an exception if the other object's value is not derived from T.
    template <
        typename T_Related,
        Detail::EnableIf<
            Detail::IsRelated<T_Related, T>::value
        > = Detail::EnableIfType::Enabled
    >
    Object(Object<T_Related> const &other);

    // Construct object containing the other object's moved value.
    // The reflected type of the object will be equivalent to that of other.
    // Throws an exception if the other object's value is not derived from T.
    template <
        typename T_Related,
        Detail::EnableIf<
            Detail::IsRelated<T_Related, T>::value
        > = Detail::EnableIfType::Enabled
    >
    Object(Object<T_Related> &&other);

    // Construct object referencing the value of other.
    // The reflected type of the object will be T_Derived.
    template <
        typename T_Derived,
        Detail::EnableIf<
            Detail::IsDerived<T_Derived, T>::value &&
            !Detail::IsReflected<T_Derived>::value
        > = Detail::EnableIfType::Enabled
    >
    Object(std::reference_wrapper<T_Derived> &&other);

    // Construct object referencing the other object's reflected value.
    // The reflected type of the object will be equivalent to that of other.
    // Throws an exception if the other object's value is not derived from T.
    template <
        template <typename> class T_Reflected,
        typename T_Related,
        Detail::EnableIf<
            Detail::IsReflected<T_Reflected<T_Related>>::value &&
            Detail::IsRelated<T_Related, T>::value
        > = Detail::EnableIfType::Enabled
    >
    Object(std::reference_wrapper<T_Reflected<T_Related>> &&other);

    template <
        template <typename> class T_Reflected,
        typename T_Related,
        Detail::EnableIf<
            Detail::IsReflected<T_Reflected<T_Related>>::value &&
            Detail::IsRelated<T_Related, T>::value
        > = Detail::EnableIfType::Enabled
    >
    Object(std::reference_wrapper<T_Reflected<T_Related> const> &&other);

    // Construct object containing an instance of type T, forwarding the
    // provided arguments to T's constructor.
    // The reflected type of the object will be T.
    template <
        typename ...T_Args,
        Detail::EnableIf<
            std::is_constructible<T, T_Args...>::value &&
            !Detail::IsReflected<T_Args...>::value &&
            !Detail::IsSameTemplate<T_Args..., std::reference_wrapper<T>>::value
        > = Detail::EnableIfType::Enabled
    >
    Object(T_Args &&...args);

    // Default constructor for Object<void>.
    // The reflected type of the object will be void.
    template <
        typename T_Void = T,
        Detail::EnableIf<
            std::is_void<T_Void>::value
        > = Detail::EnableIfType::Enabled
    >
    Object();

    // Destroy the object and its contents.
    ~Object();

//-------------------------------  Value Access  -------------------------------
public:
    // Retrieve the contained value by mutable reference.
    // Throws an exception if the contained value cannot be converted to type
    // T_Return.
    template <
        typename T_Return = Detail::DefaultReturnType<T>,
        Detail::EnableIf<
            Detail::IsRelated<T_Return, T>::value &&
            std::is_reference<T_Return>::value &&
            !std::is_const<Detail::Decompose<T_Return>>::value
        > = Detail::EnableIfType::Enabled
    >
    T_Return get();

    // Retrieve the contained value by value or constant reference.
    // Throws an exception if the contained value cannot be converted to type
    // T_Return.
    template <
        typename T_Return = Detail::DefaultReturnType<T const>,
        Detail::EnableIf<
            (!std::is_reference<T_Return>::value &&
             !std::is_void<T_Return>::value) ||
            (std::is_const<Detail::Decompose<T_Return>>::value &&
             Detail::IsRelated<T_Return, T>::value)
        > = Detail::EnableIfType::Enabled
    >
    T_Return get() const;

    // Set the contained value without changing its reflected type.
    // Throws an exception if the contained value is constant or cannot be set
    // from type T_Value.
    template <
        typename T_Value,
        Detail::EnableIf<
            !Detail::IsReflected<T_Value>::value
        > = Detail::EnableIfType::Enabled
    >
    void set(T_Value &&value);

    // Set the contained value without changing its reflected type by copy-
    // assigning the contained value of another object.
    // Throws an exception if the contained value is constant or cannot be set
    // from the other object's reflected type.
    template <
        template <typename> class T_Reflected,
        typename T_Value,
        Detail::EnableIf<
            Detail::IsReflected<T_Reflected<T_Value>>::value
        > = Detail::EnableIfType::Enabled
    >
    void set(T_Reflected<T_Value> const &value);

    // Set the contained value without changing its reflected type by move-
    // assigning the contained value of another object.
    // Throws an exception if the contained value is constant or cannot be set
    // from the other object's reflected type.
    template <
        template <typename> class T_Reflected,
        typename T_Value,
        Detail::EnableIf<
            Detail::IsReflected<T_Reflected<T_Value>>::value
        > = Detail::EnableIfType::Enabled
    >
    void set(T_Reflected<T_Value> &&value);

//-----------------------------  Type Reflection  ------------------------------
public:
    // Retrieve the qualified reflected type of the contained value.
    Type getType() const;

    // Retrieve the unqualified reflected type of the contained value.
    Type getUnqualifiedType() const;

    // Returns true if the contained value is constant, meaning it cannot be set
    // nor be retrieved by mutable reference.
    bool isConstant() const;

    // Returns true if the object contains a reference to a value not owned by
    // the object.
    bool isReference() const;

//---------------------------  Property Reflection  ----------------------------
public:
    // Retrieve a reference to the property associated with name.
    Object<void> getProperty(std::string const &name);
    Object<void> getProperty(std::string const &name) const;

//----------------------------  Private Interface  -----------------------------
private:
    // Construct object referencing the specified property of the accessed
    // owner.
    Object(Detail::Property const &property,
           Detail::Accessor const *accessor,
           Detail::Storage const &owner,
           bool constant);

//-----------------------------  Private Members  ------------------------------
private:
    template <typename T_Other>
    friend class Object;

    Detail::Storage _storage;
    Detail::Accessor const *_accessor;
};

}
//--                          End Namespace Reflect                           --
//------------------------------------------------------------------------------

#include "object.hpp"

#endif
