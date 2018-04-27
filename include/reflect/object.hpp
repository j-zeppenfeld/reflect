// Copyright (c) 2018 Johannes Zeppenfeld
// Distributed under the MIT license, see LICENSE file in repository root.

#include "detail/value_accessor.h"

//------------------------------------------------------------------------------
//--                         Begin Namespace Reflect                          --
namespace Reflect {

//------------------------------------------------------------------------------
//--                               Class Object                               --
//------------------------------------------------------------------------------

//--------------------------  Creation / Destruction  --------------------------

// Construct object containing a copy of other.
// The reflected type of the object will be T_Derived.
template <typename T>
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
Object<T>::Object(T_Derived &&other) {
    using T_Value = typename std::decay<T_Derived>::type;
    _accessor = Detail::ValueAccessor<T_Value>::instance();
    _storage.construct<T_Value>(std::forward<T_Derived>(other));
}

// Construct object containing a copy of the other object's value.
// The reflected type of the object will be equivalent to that of other.
template <typename T>
Object<T>::Object(Object<T> const &other) {
    _accessor = other._accessor->allocateCopy(other._storage, _storage);
}

// Construct object containing the other object's moved value.
// The reflected type of the object will be equivalent to that of other.
template <typename T>
Object<T>::Object(Object<T> &&other) {
    _accessor = other._accessor->allocateMove(other._storage, _storage);
}

// Construct object containing a copy of the other object's value.
// The reflected type of the object will be equivalent to that of other.
// Throws an exception if other's reflected type is not derived from T.
template <typename T>
template <
    typename T_Related,
    Detail::EnableIf<
        Detail::IsRelated<T_Related, T>::value
    >...
>
Object<T>::Object(Object<T_Related> const &other) {
    // TODO: Verify that other's reflected type derives from T.
    _accessor = other._accessor->allocateCopy(other._storage, _storage);
}

// Construct object containing the other object's moved value.
// The reflected type of the object will be equivalent to that of other.
// Throws an exception if other's reflected type is not derived from T.
template <typename T>
template <
    typename T_Related,
    Detail::EnableIf<
        Detail::IsRelated<T_Related, T>::value
    >...
>
Object<T>::Object(Object<T_Related> &&other) {
    // TODO: Verify that other's reflected type derives from T.
    _accessor = other._accessor->allocateMove(other._storage, _storage);
}

// Construct object referencing the value of other.
// The reflected type of the object will be T_Derived.
template <typename T>
template <
    typename T_Derived,
    Detail::EnableIf<
        Detail::IsDerived<T_Derived, T>::value &&
        !Detail::IsSameTemplate<T_Derived, Object<T>>::value &&
        !Detail::IsSameTemplate<T_Derived, Value<T>>::value &&
        !Detail::IsSameTemplate<T_Derived, Reference<T>>::value
    >...
>
Object<T>::Object(std::reference_wrapper<T_Derived> other) {
    using T_Value = Detail::Decompose<T_Derived>;
    _accessor = Detail::ValueAccessor<T_Value &>::instance();
    _storage.construct<T_Value *>(&other.get());
}

// Construct object referencing the other object's value.
// The reflected type of the object will be equivalent to that of other.
// Throws an exception if other's reflected type is not derived from T.
template <typename T>
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
Object<T>::Object(std::reference_wrapper<T_Reflected> other) {
    // TODO: Verify that other's reflected type derives from T.
    _accessor = other.get()._accessor->allocateReference(
        other.get()._storage, _storage, std::is_const<T_Reflected>::value
    );
}

// Construct object referencing the other object's value.
// The reflected type of the object will be equivalent to that of other.
template <typename T>
Object<T>::Object(std::reference_wrapper<Object<T>> other) {
    _accessor = other.get()._accessor->allocateReference(
        other.get()._storage, _storage, false
    );
}

template <typename T>
Object<T>::Object(std::reference_wrapper<Object<T> const> other) {
    _accessor = other.get()._accessor->allocateReference(
        other.get()._storage, _storage, true
    );
}

// Construct object containing an instance of T, forwarding the provided
// arguments to T's constructor.
// The reflected type of the object will be T.
template <typename T>
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
Object<T>::Object(T_Args &&...args) {
    _accessor = Detail::ValueAccessor<T>::instance();
    _storage.construct<T>(std::forward<T_Args>(args)...);
}

// Default constructor for Object<void>.
// The reflected type of the object will be void.
template <typename T>
template <
    typename T_Void,
    Detail::EnableIf<
        std::is_void<T_Void>::value
    >...
>
Object<T>::Object() {
    _accessor = Detail::ValueAccessor<void>::instance();
}

// Destroy the object and its contents.
template <typename T>
Object<T>::~Object() {
    _accessor->deallocate(_storage);
}

}
//--                          End Namespace Reflect                           --
//------------------------------------------------------------------------------
