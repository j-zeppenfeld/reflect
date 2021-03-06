// Copyright (c) 2019 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#include "type.h"

#include "detail/buffer.h"
#include "detail/type_info.h"
#include "detail/value_accessor.h"

//------------------------------------------------------------------------------
//--                         Begin Namespace Reflect                          --
namespace Reflect {

//------------------------------------------------------------------------------
//--                               Class Object                               --
//------------------------------------------------------------------------------

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
    >
>
Object<T>::Object(T_Derived &&other) {
    using T_Decayed = typename std::decay<T_Derived>::type;
    _accessor = Detail::ValueAccessor<T_Decayed>::construct(
        _storage, std::forward<T_Derived>(other)
    );
}

// Construct object containing a copy of the other object's value.
// The reflected type of the object will be equivalent to that of other.
template <typename T>
Object<T>::Object(Object<T> const &other) {
    _accessor = other._accessor->constructCopy(_storage, other._storage);
}

// Construct object containing the other object's moved value.
// The reflected type of the object will be equivalent to that of other.
template <typename T>
Object<T>::Object(Object<T> &&other) {
    _accessor = other._accessor->constructMove(_storage, other._storage);
}

// Construct object containing a copy of the other object's value.
// The reflected type of the object will be equivalent to that of other.
// Throws an exception if other's reflected type is not derived from T.
template <typename T>
template <
    typename T_Related,
    Detail::EnableIf<
        Detail::IsRelated<T_Related, T>::value
    >
>
Object<T>::Object(Object<T_Related> const &other) {
    // TODO: Verify that other's reflected type derives from T.
    _accessor = other._accessor->constructCopy(_storage, other._storage);
}

// Construct object containing the other object's moved value.
// The reflected type of the object will be equivalent to that of other.
// Throws an exception if other's reflected type is not derived from T.
template <typename T>
template <
    typename T_Related,
    Detail::EnableIf<
        Detail::IsRelated<T_Related, T>::value
    >
>
Object<T>::Object(Object<T_Related> &&other) {
    // TODO: Verify that other's reflected type derives from T.
    _accessor = other._accessor->constructMove(_storage, other._storage);
}

// Construct object referencing the value of other.
// The reflected type of the object will be T_Derived.
template <typename T>
template <
    typename T_Derived,
    Detail::EnableIf<
        Detail::IsDerived<T_Derived, T>::value &&
        !Detail::IsReflected<T_Derived>::value
    >
>
Object<T>::Object(std::reference_wrapper<T_Derived> &&other) {
    using T_Decomposed = Detail::Decompose<T_Derived>;
    _accessor = Detail::ValueAccessor<T_Decomposed &>::construct(
        _storage, other.get()
    );
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
    >
>
Object<T>::Object(std::reference_wrapper<T_Reflected<T_Related>> &&other) {
    // TODO: Verify that other's reflected type derives from T.
    _accessor = other.get()._accessor->constructReference(
        _storage, other.get()._storage, false
    );
}

template <typename T>
template <
    template <typename> class T_Reflected,
    typename T_Related,
    Detail::EnableIf<
        Detail::IsReflected<T_Reflected<T_Related>>::value &&
        Detail::IsRelated<T_Related, T>::value
    >
>
Object<T>::Object(
    std::reference_wrapper<T_Reflected<T_Related> const> &&other
) {
    // TODO: Verify that other's reflected type derives from T.
    _accessor = other.get()._accessor->constructReference(
        _storage, other.get()._storage, true
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
    >
>
Object<T>::Object(T_Args &&...args) {
    _accessor = Detail::ValueAccessor<T>::construct(
        _storage, std::forward<T_Args>(args)...
    );
}

// Default constructor for Object<void>.
// The reflected type of the object will be void.
template <typename T>
template <
    typename T_Void,
    Detail::EnableIf<
        std::is_void<T_Void>::value
    >
>
Object<T>::Object() {
    _accessor = Detail::ValueAccessor<void>::construct(_storage);
}

// Destroy the object and its contents.
template <typename T>
Object<T>::~Object() {
    _accessor->destruct(_storage);
}

//-------------------------------  Value Access  -------------------------------

// Retrieve the contained value by mutable reference.
// Throws an exception if the contained value cannot be converted to type
// T_Return.
template <typename T>
template <
    typename T_Return,
    Detail::EnableIf<
        Detail::IsRelated<T_Return, T>::value &&
        std::is_reference<T_Return>::value &&
        !std::is_const<Detail::Decompose<T_Return>>::value
    >
>
T_Return Object<T>::get() {
    using T_Decayed = typename std::decay<T_Return>::type;

    // Retrieve value from storage using the accessor.
    return *static_cast<T_Decayed *>(
        _accessor->getAs(
            _storage, Detail::TypeInfo::instance<T_Decayed>()
        )
    );
}

// Retrieve the contained value by value or constant reference.
// Throws an exception if the contained value cannot be converted to type
// T_Return.
template <typename T>
template <
    typename T_Return,
    Detail::EnableIf<
        (!std::is_reference<T_Return>::value &&
         !std::is_void<T_Return>::value) ||
        (std::is_const<Detail::Decompose<T_Return>>::value &&
         Detail::IsRelated<T_Return, T>::value)
    >
>
T_Return Object<T>::get() const {
    using T_Decayed = typename std::decay<T_Return>::type;

    struct Impl {
        // Retrieve value from storage by constant reference using the accessor.
        static T_Decayed const &get(std::true_type,
                                    Detail::Accessor const *accessor,
                                    Detail::Storage const &storage) {
            return *static_cast<T_Decayed const *>(
                accessor->getAsConst(
                    storage, Detail::TypeInfo::instance<T_Decayed>()
                )
            );
        }

        // Retrieve value from storage by value using the accessor.
        static T_Decayed get(std::false_type,
                             Detail::Accessor const *accessor,
                             Detail::Storage const &storage) {
            // Buffer into which the accessor can construct an instance of the
            // returned type. This is needed if the accessor can only return by
            // value.
            Detail::Buffer<T_Decayed> buffer;

            // Retrieve value from storage using the accessor.
            void const *value = accessor->getAsConst(
                storage, Detail::TypeInfo::instance<T_Decayed>(), &buffer
            );
            if(buffer.isConstructed()) {
                return std::move(buffer.getValue());
            } else {
                return *static_cast<T_Decayed const *>(value);
            }
        }
    };

    // Retrieve by value or constant reference.
    return Impl::get(std::is_reference<T_Return>(),
                     _accessor,
                     _storage);
}

// Set the contained value without changing its reflected type.
// Throws an exception if the contained value is constant or cannot be set
// from type T_Value.
template <typename T>
template <
    typename T_Value,
    Detail::EnableIf<
        !Detail::IsReflected<T_Value>::value
    >
>
void Object<T>::set(T_Value &&value) {
    using T_Decayed = typename std::decay<T_Value>::type;

    struct Impl {
        // Copy-assign value to storage using the accessor.
        static void set(std::true_type,
                        Detail::Accessor const *accessor,
                        Detail::Storage &storage,
                        T_Decayed const &value) {
            accessor->setAs(storage,
                            Detail::TypeInfo::instance<T_Decayed>(),
                            &value);
        }

        // Move-assign value to storage using the accessor.
        static void set(std::false_type,
                        Detail::Accessor const *accessor,
                        Detail::Storage &storage,
                        T_Decayed &value) {
            accessor->moveAs(storage,
                             Detail::TypeInfo::instance<T_Decayed>(),
                             &value);
        }
    };

    // Copy or move value depending on whether it is a reference.
    Impl::set(std::is_lvalue_reference<T_Value>(),
              _accessor,
              _storage,
              value);
}

// Set the contained value without changing its reflected type by copy-
// assigning the contained value of another object.
// Throws an exception if the contained value is constant or cannot be set
// from the other object's reflected type.
template <typename T>
template <
    template <typename> class T_Reflected,
    typename T_Value,
    Detail::EnableIf<
        Detail::IsReflected<T_Reflected<T_Value>>::value
    >
>
void Object<T>::set(T_Reflected<T_Value> const &value) {
    // Copy-assign value to storage using the accessor.
    _accessor->setAs(_storage, value._accessor, value._storage);
}

// Set the contained value without changing its reflected type by move-
// assigning the contained value of another object.
// Throws an exception if the contained value is constant or cannot be set
// from the other object's reflected type.
template <typename T>
template <
    template <typename> class T_Reflected,
    typename T_Value,
    Detail::EnableIf<
        Detail::IsReflected<T_Reflected<T_Value>>::value
    >
>
void Object<T>::set(T_Reflected<T_Value> &&value) {
    // Move-assign value to storage using the accessor.
    _accessor->moveAs(_storage, value._accessor, value._storage);
}

//-----------------------------  Type Reflection  ------------------------------

// Retrieve the qualified reflected type of the contained value.
template <typename T>
Type Object<T>::getType() const {
    return {
        _accessor->getTypeInfo(),
        _accessor->isConstant(),
        _accessor->isReference()
    };
}

// Retrieve the unqualified reflected type of the contained value.
template <typename T>
Type Object<T>::getUnqualifiedType() const {
    return {
        _accessor->getTypeInfo(),
        false,
        false
    };
}

// Returns true if the contained value is constant.
template <typename T>
bool Object<T>::isConstant() const {
    return _accessor->isConstant();
}

// Returns true if the object contains a reference.
template <typename T>
bool Object<T>::isReference() const {
    return _accessor->isReference();
}

}
//--                          End Namespace Reflect                           --
//------------------------------------------------------------------------------
