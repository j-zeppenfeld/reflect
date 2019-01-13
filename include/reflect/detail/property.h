// Copyright (c) 2019 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#ifndef REFLECT_DETAIL_PROPERTY_H
#define REFLECT_DETAIL_PROPERTY_H

// std::unique_ptr
#include <memory>
#include <string>

//------------------------------------------------------------------------------
//--                     Begin Namespace Reflect::Detail                      --
namespace Reflect { namespace Detail {

// Uses.
class Accessor;
template <typename T, typename T_Value, typename ...T_Accessors>
class PropertyAccessor;
class Storage;

//------------------------------------------------------------------------------
//--                              Class Property                              --
//------------------------------------------------------------------------------
// Contains information about a property of a type.
class Property {
public:
    Property(std::string name,
             std::unique_ptr<PropertyAccessor<void, void>> mutableAccessor,
             std::unique_ptr<PropertyAccessor<void, void>> constantAccessor);

    Property(Property &&other);

    ~Property();

//-----------------------------  Public Interface  -----------------------------
public:
    // Retrieve the name by which the property was registered.
    std::string const &getName() const { return _name; }

//-------------------------------  Construction  -------------------------------
public:
    // Construct a reference to property of owner, which must be of the
    // property's owner type, within storage.
    // Returns an accessor for the property reference constructed in storage.
    Accessor const *construct(Storage &storage,
                              void *owner, bool constant, bool temporary) const;

//-----------------------------  Private Members  ------------------------------
private:
    std::string _name;
    std::unique_ptr<PropertyAccessor<void, void>> _mutableAccessor;
    std::unique_ptr<PropertyAccessor<void, void>> _constantAccessor;
};

} }
//--                      End Namespace Reflect::Detail                       --
//------------------------------------------------------------------------------

#endif
