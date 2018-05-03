// Copyright (c) 2018 Johannes Zeppenfeld
// Distributed under the MIT license, see LICENSE file in repository root.

#include "detail/type_info.h"
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
        !Detail::IsReflected<T_Derived>::value &&
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
        !Detail::IsReflected<T_Derived>::value
    >...
>
Object<T>::Object(std::reference_wrapper<T_Derived> &&other) {
    using T_Value = Detail::Decompose<T_Derived>;
    _accessor = Detail::ValueAccessor<T_Value &>::instance();
    _storage.construct<T_Value *>(&other.get());
}

// Construct object referencing the other object's reflected value.
// The reflected type of the object will be equivalent to that of other.
// Throws an exception if other's reflected type is not derived from T.
template <typename T>
template <
    template <typename> class T_Reflected,
    typename T_Related,
    Detail::EnableIf<
        Detail::IsReflected<T_Reflected<T_Related>>::value &&
        Detail::IsRelated<T_Related, T>::value
    >...
>
Object<T>::Object(std::reference_wrapper<T_Reflected<T_Related>> &&other) {
    // TODO: Verify that other's reflected type derives from T.
    _accessor = other.get()._accessor->allocateReference(
        other.get()._storage, _storage, false
    );
}

template <typename T>
template <
    template <typename> class T_Reflected,
    typename T_Related,
    Detail::EnableIf<
        Detail::IsReflected<T_Reflected<T_Related>>::value &&
        Detail::IsRelated<T_Related, T>::value
    >...
>
Object<T>::Object(
    std::reference_wrapper<T_Reflected<T_Related> const> &&other
) {
    // TODO: Verify that other's reflected type derives from T.
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
        !Detail::IsReflected<T_Args...>::value &&
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

//-------------------------------  Value Access  -------------------------------

// Retrieve the contained value by value.
// Throws an exception if the contained value cannot be converted to type
// T_Related.
template <typename T>
template <
    typename T_Related,
    Detail::EnableIf<
        Detail::IsRelated<T_Related, T>::value &&
        !std::is_void<T_Related>::value &&
        !std::is_reference<T_Related>::value
    >...
>
T_Related Object<T>::get() const {
    using T_Value = typename std::decay<T_Related>::type;

    // If the accessed type does not exactly match T_Value, it is not possible
    // to create an appropriate buffer for the accessor's get method here.
    // Without a buffer, the accessor must return a reference, which can most
    // easily be retrieved using the get method returning a constant reference.
    if(_accessor->getTypeInfo() != Detail::TypeInfo::instance<T_Value>()) {
        return get<T_Value const &>();
    }

    // Buffer into which the accessor can construct an instance of the returned
    // type. This is needed if the accessor can only return by value.
    struct Buffer {
        union { T_Value _value; };
        bool _constructed;

        Buffer() : _constructed(false) { }
        ~Buffer() { if(_constructed) _value.~T_Value(); }
    } buffer;

    // Retrieve value from storage using the accessor.
    auto value = _accessor->get(_storage, &buffer._value);
    if(value._value == &buffer._value) {
        buffer._constructed = true;
        return std::move(buffer._value);
    } else {
        return *static_cast<T_Value const *>(value._value);
    }
}

// Retrieve the contained value by mutable reference.
// Throws an exception if the contained value cannot be converted to type
// T_Related.
template <typename T>
template <
    typename T_Related,
    Detail::EnableIf<
        Detail::IsRelated<T_Related, T>::value &&
        std::is_reference<T_Related>::value &&
        !std::is_const<Detail::Decompose<T_Related>>::value
    >...
>
T_Related Object<T>::get() {
    using T_Value = typename std::decay<T_Related>::type;

    // TODO: Allow type conversion.
    if(_accessor->getTypeInfo() != Detail::TypeInfo::instance<T_Value>()) {
        throw std::runtime_error(
            "Accessing object value as wrong type."
        );
    }

    // Retrieve value from storage using the accessor.
    auto value = _accessor->get(_storage);
    if(value._constant) {
        throw std::runtime_error(
            "Retrieving mutable reference to constant object value."
        );
    }
    return *static_cast<T_Value *>(value._value);
}

// Retrieve the contained value by constant reference.
// Throws an exception if the contained value cannot be converted to type
// T_Related.
template <typename T>
template <
    typename T_Related,
    Detail::EnableIf<
        Detail::IsRelated<T_Related, T>::value &&
        std::is_reference<T_Related>::value &&
        std::is_const<Detail::Decompose<T_Related>>::value
    >...
>
T_Related Object<T>::get() const {
    using T_Value = typename std::decay<T_Related>::type;

    // TODO: Allow type conversion.
    if(_accessor->getTypeInfo() != Detail::TypeInfo::instance<T_Value>()) {
        throw std::runtime_error(
            "Accessing object value as wrong type."
        );
    }

    // Retrieve value from storage using the accessor.
    auto value = _accessor->get(_storage);
    return *static_cast<T_Value const *>(value._value);
}

// Set the contained value without changing its reflected type.
// Throws an exception if the contained value is constant or cannot be set
// from type T_Derived.
template <typename T>
template <
    typename T_Derived,
    Detail::EnableIf<
        Detail::IsDerived<T_Derived, T>::value &&
        !Detail::IsReflected<T_Derived>::value
    >...
>
void Object<T>::set(T_Derived &&value) {
    using T_Value = typename std::decay<T_Derived>::type;

    // TODO: Allow type conversion.
    if(_accessor->getTypeInfo() != Detail::TypeInfo::instance<T_Value>()) {
        throw std::runtime_error(
            "Setting object value from incompatible type."
        );
    }

    // Assign value to storage using the accessor.
    if(!(std::is_lvalue_reference<T_Derived>::value
         ? _accessor->set(_storage, &value)
         : _accessor->move(_storage, &value)
        )
    ) {
        throw std::runtime_error(
            "Setting constant object value."
        );
    }
}

// Set the contained value without changing its reflected type by copy-
// assigning the contained value of another object.
// Throws an exception if the contained value is constant or cannot be set
// from the other object's reflected type.
template <typename T>
template <
    template <typename> class T_Reflected,
    typename T_Related,
    Detail::EnableIf<
        Detail::IsReflected<T_Reflected<T_Related>>::value &&
        Detail::IsRelated<T_Related, T>::value
    >...
>
void Object<T>::set(T_Reflected<T_Related> const &value) {
    // TODO: Allow type conversion.
    if(_accessor->getTypeInfo() != value._accessor->getTypeInfo()) {
        throw std::runtime_error(
            "Setting object value from incompatible type."
        );
    }

    // Retrieve value from storage using the accessor.
    auto accessed = value._accessor->get(value._storage);

    // Assign value to storage using the accessor.
    if(!_accessor->set(_storage, accessed._value)) {
        throw std::runtime_error(
            "Setting constant object value."
        );
    }
}

// Set the contained value without changing its reflected type by move-
// assigning the contained value of another object.
// Throws an exception if the contained value is constant or cannot be set
// from the other object's reflected type.
template <typename T>
template <
    template <typename> class T_Reflected,
    typename T_Related,
    Detail::EnableIf<
        Detail::IsReflected<T_Reflected<T_Related>>::value &&
        Detail::IsRelated<T_Related, T>::value
    >...
>
void Object<T>::set(T_Reflected<T_Related> &&value) {
    // TODO: Allow type conversion.
    if(_accessor->getTypeInfo() != value._accessor->getTypeInfo()) {
        throw std::runtime_error(
            "Setting object value from incompatible type."
        );
    }

    // Retrieve value from storage using the accessor.
    auto accessed = value._accessor->get(value._storage);

    // Assign value to storage using the accessor.
    if(!(accessed._constant
         ? _accessor->set(_storage, accessed._value)
         : _accessor->move(_storage, accessed._value)
        )
    ) {
        throw std::runtime_error(
            "Setting constant object value."
        );
    }
}

}
//--                          End Namespace Reflect                           --
//------------------------------------------------------------------------------
