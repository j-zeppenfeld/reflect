// Copyright (c) 2018 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#include "detail/type_info.h"

#include <type_traits>

//------------------------------------------------------------------------------
//--                         Begin Namespace Reflect                          --
namespace Reflect {

//------------------------------------------------------------------------------
//--                                Class Type                                --
//------------------------------------------------------------------------------

//---------------------------  Non-Member Functions  ---------------------------

// Create a type instance for type T.
template <typename T>
Type getType() {
    return {
        Detail::TypeInfo::instance<typename std::decay<T>::type>(),
        std::is_const<typename std::remove_reference<T>::type>::value,
        std::is_reference<T>::value
    };
}

}
//--                          End Namespace Reflect                           --
//------------------------------------------------------------------------------
