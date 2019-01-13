// Copyright (c) 2019 Johannes Zeppenfeld
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

//--------------------------------  Properties  --------------------------------
public:
    // Register a member pointer or accessor function as a property of type T
    // with the specified name.
    template <typename T_Accessor>
    Register &property(std::string name, T_Accessor &&accessor);

    // Register a pair of get and set accessor functions as a property of type T
    // with the specified name.
    template <typename T_GetAccessor, typename T_SetAccessor>
    Register &property(std::string name,
                       T_GetAccessor &&getAccessor,
                       T_SetAccessor &&setAccessor);

    // Register a pair of const and non-const getter methods as a property of
    // type T with the specified name.
    template <typename T_Owner, typename T_Mutable, typename T_Constant>
    Register &property(std::string name,
                       T_Mutable (T_Owner::*getMutableFunction)(),
                       T_Constant (T_Owner::*getConstantFunction)() const);

    // Register a pair of const and non-const getter methods together with a
    // set accessor function as a property of type T with the specified name.
    template <typename T_Owner, typename T_Mutable, typename T_Constant,
              typename T_SetAccessor>
    Register &property(std::string name,
                       T_Mutable (T_Owner::*getMutableFunction)(),
                       T_Constant (T_Owner::*getConstantFunction)() const,
                       T_SetAccessor &&setAccesor);
};

}
//--                          End Namespace Reflect                           --
//------------------------------------------------------------------------------

#include "register.hpp"

#endif
