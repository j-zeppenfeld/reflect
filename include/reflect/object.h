// Copyright (c) 2018 Johannes Zeppenfeld
// Distributed under the MIT license, see LICENSE file in repository root.

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
template <typename T> class Reference;
template <typename T> class Value;
namespace Detail { class Accessor; }

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

//--------------------------  Creation / Destruction  --------------------------
public:
    // Construct object containing a copy of other.
    // The reflected type of the object will be T_Derived.
    template <
        typename T_Derived,
        Detail::EnableIf<
            Detail::IsDerived<T_Derived, T>::value &&
            std::is_constructible<T_Derived, T_Derived>::value &&
            !Detail::IsSameTemplate<T_Derived, Object<T>>::value &&
            !Detail::IsSameTemplate<T_Derived, Value<T>>::value &&
            !Detail::IsSameTemplate<T_Derived, Reference<T>>::value &&
            !Detail::IsSameTemplate<T_Derived, std::reference_wrapper<T>>::value
        >...
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
        >...
    >
    Object(Object<T_Related> const &other);

    // Construct object containing the other object's moved value.
    // The reflected type of the object will be equivalent to that of other.
    // Throws an exception if the other object's value is not derived from T.
    template <
        typename T_Related,
        Detail::EnableIf<
            Detail::IsRelated<T_Related, T>::value
        >...
    >
    Object(Object<T_Related> &&other);

    // Construct object referencing the value of other.
    // The reflected type of the object will be T_Derived.
    template <
        typename T_Derived,
        Detail::EnableIf<
            Detail::IsDerived<T_Derived, T>::value &&
            !Detail::IsSameTemplate<T_Derived, Object<T>>::value &&
            !Detail::IsSameTemplate<T_Derived, Value<T>>::value &&
            !Detail::IsSameTemplate<T_Derived, Reference<T>>::value
        >...
    >
    Object(std::reference_wrapper<T_Derived> other);

    // Construct object referencing the other object's value.
    // The reflected type of the object will be equivalent to that of other.
    // Throws an exception if the other object's value is not derived from T.
    template <
        typename T_Reflected,
        Detail::EnableIf<
            Detail::IsRelated<typename T_Reflected::element_type, T>::value &&
            (Detail::IsSameTemplate<T_Reflected, Object<T>>::value ||
             Detail::IsSameTemplate<T_Reflected, Value<T>>::value ||
             Detail::IsSameTemplate<T_Reflected, Reference<T>>::value
            )
        >...
    >
    Object(std::reference_wrapper<T_Reflected> other);

    // Construct object referencing the other object's value.
    // The reflected type of the object will be equivalent to that of other.
    Object(std::reference_wrapper<Object<T>> other);

    Object(std::reference_wrapper<Object<T> const> other);

    // Construct object containing an instance of type T, forwarding the
    // provided arguments to T's constructor.
    // The reflected type of the object will be T.
    template <
        typename ...T_Args,
        Detail::EnableIf<
            std::is_constructible<T, T_Args...>::value &&
            !Detail::IsSameTemplate<T_Args..., Object<T>>::value &&
            !Detail::IsSameTemplate<T_Args..., Value<T>>::value &&
            !Detail::IsSameTemplate<T_Args..., Reference<T>>::value &&
            !Detail::IsSameTemplate<T_Args..., std::reference_wrapper<T>>::value
        >...
    >
    Object(T_Args &&...args);

    // Default constructor for Object<void>.
    // The reflected type of the object will be void.
    template <
        typename T_Void = T,
        Detail::EnableIf<
            std::is_void<T_Void>::value
        >...
    >
    Object();

    // Destroy the object and its contents.
    ~Object();

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
