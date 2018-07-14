// Copyright (c) 2018 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#ifndef REFLECT_REGISTER_H
#define REFLECT_REGISTER_H

#include <string>
#include <type_traits>

//------------------------------------------------------------------------------
//--                         Begin Namespace Reflect                          --
namespace Reflect {

//------------------------------------------------------------------------------
//--                              Class Register                              --
//------------------------------------------------------------------------------
template <typename T>
class Register {
public:
    static_assert(std::is_same<T, typename std::decay<T>::type>::value,
                  "Registration must be of unqualified type.");

//-------------------------------  Registration  -------------------------------
public:
    // Register information about type T without associating it with a name.
    Register() = default;

    // Register information about type T, associating it globally with name.
    // Throws an exception if name has already been used to globally register
    // a different type.
    Register(std::string name);

//-------------------------------  Base Classes  -------------------------------
public:
    // Register type T_Base as a base class of type T.
    template <typename T_Base>
    Register &base();

//-------------------------------  Conversions  --------------------------------
public:
    // Register a conversion from type T to type T_Target.
    template <typename T_Target>
    Register &conversion();

    // Register a conversion from type T to type T_Target, using the specified
    // conversion method.
    template <typename T_Target, typename T_Base, typename T_Ret>
    Register &conversion(T_Ret (T_Base::*method)() const);

    // Register a conversion from type T to type T_Target, using the specified
    // conversion function.
    template <typename T_Target, typename T_Func>
    Register &conversion(T_Func &&function);
};

}
//--                          End Namespace Reflect                           --
//------------------------------------------------------------------------------

#include "register.hpp"

#endif
