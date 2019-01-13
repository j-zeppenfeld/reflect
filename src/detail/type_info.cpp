// Copyright (c) 2019 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#include "reflect/detail/type_info.h"

// std::runtime_error
#include <stdexcept>

//------------------------------------------------------------------------------
//--                     Begin Namespace Reflect::Detail                      --
namespace Reflect { namespace Detail {

//------------------------------------------------------------------------------
//--                              Class TypeInfo                              --
//------------------------------------------------------------------------------

//--------------------------------  Properties  --------------------------------

// Retrieve the property with name that has been registered for the type.
Property const &TypeInfo::getProperty(std::string const &name) const {
    for(auto &property : _properties) {
        if(property.getName() == name) {
            return property;
        }
    }

    throw std::runtime_error(
        "Property '" + name + "' not registered for type '" + _name + "'."
    );
}

} }
//--                      End Namespace Reflect::Detail                       --
//------------------------------------------------------------------------------
