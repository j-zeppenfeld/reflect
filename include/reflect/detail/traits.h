// Copyright (c) 2018 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#ifndef REFLECT_DETAIL_TRAITS_H
#define REFLECT_DETAIL_TRAITS_H

#include <type_traits>

//------------------------------------------------------------------------------
//--                         Begin Namespace Reflect                          --
namespace Reflect {

// Uses.
template <typename T> class Object;
template <typename T> class Value;
template <typename T> class Reference;

//------------------------------------------------------------------------------
//--                          Begin Namespace Detail                          --
namespace Detail {

// Simplified enable_if for better usability.
enum struct EnableIfType { };

template <bool condition>
using EnableIf = typename std::enable_if<condition, EnableIfType>::type;

// Determines whether T_Derived is derived from or equivalent to T_Base.
// Evaluates to false if more than two types are specified.
// Note that void is considered a base of all types.
template <typename, typename ...>
struct IsDerived : std::false_type { };
template <typename T_Derived, typename T_Base>

struct IsDerived<T_Derived, T_Base>
: std::conditional<
    std::is_void<typename std::decay<T_Base>::type>::value ||
    std::is_base_of<typename std::decay<T_Base>::type,
                    typename std::decay<T_Derived>::type>::value ||
    std::is_same<typename std::decay<T_Base>::type,
                 typename std::decay<T_Derived>::type>::value,
    std::true_type, std::false_type
>::type { };

// Determines whether the first type is related to the second type in a way that
// could be resolved by the reflection system, i.e., if one type is derived from
// or is equivalent to the other.
// Evaluates to false if more than two types are specified.
// Note that void is considered a base of all types.
template <typename, typename ...>
struct IsRelated : std::false_type { };

template <typename T_Lhs, typename T_Rhs>
struct IsRelated<T_Lhs, T_Rhs>
: std::conditional<
    IsDerived<T_Lhs, T_Rhs>::value ||
    IsDerived<T_Rhs, T_Lhs>::value,
    std::true_type, std::false_type
>::type { };

// Determines whether the first type is of the same template class as the
// second, disregarding the type's template parameters.
// Evaluates to false if more than two types are specified.
template <typename, typename ...>
struct IsSameTemplateImpl : std::false_type { };

template <template <typename ...> class T, typename ...U, typename ...V>
struct IsSameTemplateImpl<T<U...>, T<V...>> : std::true_type { };

template <typename ...T_Args>
struct IsSameTemplate : IsSameTemplateImpl<typename std::decay<T_Args>::type...>
{ };

// Determines whether the specified type contains a reflected value, i.e., is
// one of Reflect::Object, Reflect::Value or Reflect::Reference.
// Evaluates to false if more than one type is specified.
template <typename ...>
struct IsReflected : std::false_type { };

template <typename T>
struct IsReflected<T>
: std::conditional<
    IsSameTemplate<T, Object<void>>::value ||
    IsSameTemplate<T, Value<void>>::value ||
    IsSameTemplate<T, Reference<void>>::value,
    std::true_type, std::false_type
>::type { };

// Applies lvalue-to-rvalue, array-to-pointer and function-to-pointer
// conversions to type T. This is similar to std::decay, but does not remove
// cv-qualifiers.
template <typename T>
struct DecomposeImpl {
    using U = typename std::remove_reference<T>::type;
    using type = typename std::conditional<
        std::is_array<U>::value,
        typename std::remove_extent<U>::type *,
        typename std::conditional< 
            std::is_function<U>::value,
            typename std::add_pointer<U>::type,
            U
        >::type
    >::type;
};

template <typename T>
using Decompose = typename DecomposeImpl<T>::type;

// Determine the default return type to use for type T.
// Scalar types (including void) are returned by unqualified value, all other
// types are returned by qualified reference.
template <typename T>
using DefaultReturnType = typename std::conditional<
    std::is_scalar<T>::value || std::is_void<T>::value,
    std::remove_cv<T>,
    std::add_lvalue_reference<T>
>::type::type;

} }
//--                      End Namespace Reflect::Detail                       --
//------------------------------------------------------------------------------

#endif
