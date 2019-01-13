// Copyright (c) 2019 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#include "reflect/detail/property.h"

#include "reflect/detail/property_accessor.h"

//------------------------------------------------------------------------------
//--                     Begin Namespace Reflect::Detail                      --
namespace Reflect { namespace Detail {

//------------------------------------------------------------------------------
//--                              Class Property                              --
//------------------------------------------------------------------------------

Property::Property(
    std::string name,
    std::unique_ptr<PropertyAccessor<void, void>> mutableAccessor,
    std::unique_ptr<PropertyAccessor<void, void>> constantAccessor
)
: _name(std::move(name))
, _mutableAccessor(std::move(mutableAccessor))
, _constantAccessor(std::move(constantAccessor)) { }

Property::Property(Property &&other) = default;

Property::~Property() = default;

//-------------------------------  Construction  -------------------------------

// Construct a reference to property of owner within storage.
Accessor const *Property::construct(
    Storage &storage, void *owner, bool constant, bool temporary
) const {
    Accessor const *result;
    if(temporary) {
        if(constant) {
            result = _constantAccessor->access(storage, owner);
        } else {
            result = _mutableAccessor->access(storage, owner);
        }
    } else {
        if(constant) {
            result = _constantAccessor->construct(storage, owner);
        } else {
            result = _mutableAccessor->construct(storage, owner);
        }
    }
    if(!result) {
        throw std::runtime_error(
            "Could not access property '" + _name + "'."
        );
    }
    return result;
}

} }
//--                      End Namespace Reflect::Detail                       --
//------------------------------------------------------------------------------
