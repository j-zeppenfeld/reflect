// Copyright (c) 2019 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#ifndef REFLECT_DETAIL_TYPEINFO_H
#define REFLECT_DETAIL_TYPEINFO_H

#include "base.h"
//#include "constant.h"
//#include "constructor.h"
#include "conversion.h"
//#include "extension.h"
//#include "function.h"
#include "property.h"

#include "iterator_range.h"

#include <string>
#include <typeinfo>
#include <type_traits>
#include <vector>

//------------------------------------------------------------------------------
//--                     Begin Namespace Reflect::Detail                      --
namespace Reflect { namespace Detail {

//------------------------------------------------------------------------------
//--                              Class TypeInfo                              --
//------------------------------------------------------------------------------
// Contains all registered information for a type.
class TypeInfo {
public:
    // Retrieve the global type information instance of type T.
    template <typename T>
    static TypeInfo const *instance() {
        return mutableInstance<T>();
    }

//-----------------------------  Public Interface  -----------------------------
public:
    // Retrieve the shortest name by which the type has been registered.
    std::string const &getName() const { return _name; }

//-------------------------------  Base Classes  -------------------------------
public:
    // Iterate over all registered base classes of the type.
    using BaseIterator = std::vector<Base>::const_iterator;
    IteratorRange<BaseIterator> getBases() const {
        return { beginBases(), endBases() };
    }
    BaseIterator beginBases() const {
        return _bases.begin();
    }
    BaseIterator endBases() const {
        return _bases.end();
    }

//-------------------------------  Conversions  --------------------------------
public:
    // Iterate over all registered conversions from the type to other types.
    using ConversionIterator = std::vector<Conversion>::const_iterator;
    IteratorRange<ConversionIterator> getConversions() const {
        return { beginConversions(), endConversions() };
    }
    ConversionIterator beginConversions() const {
        return _conversions.begin();
    }
    ConversionIterator endConversions() const {
        return _conversions.end();
    }

//--------------------------------  Properties  --------------------------------
public:
    // Retrieve the property with name that has been registered for the type.
    // Throws an exception if no property with name has been registered.
    Property const &getProperty(std::string const &name) const;

    // Iterate over all registered properties of the type.
    using PropertyIterator = std::vector<Property>::const_iterator;
    IteratorRange<PropertyIterator> getProperties() const {
        return { beginProperties(), endProperties() };
    }
    PropertyIterator beginProperties() const {
        return _properties.begin();
    }
    PropertyIterator endProperties() const {
        return _properties.end();
    }

//-------------------------------  Registration  -------------------------------
public:
    // Retrieve the global type information instance of type T.
    template <typename T>
    static TypeInfo *mutableInstance() {
        static_assert(
            std::is_same<T, typename std::decay<T>::type>::value,
            "Internal error: Type information instance must be decayed."
        );

        static TypeInfo typeInfo(typeid(T));
        return &typeInfo;
    }

    // Register a name for the type.
    void registerName(std::string name) {
        _name = std::move(name);
        _nameSet = true;
    }

    // Register a base class for the type.
    void registerBase(Base base) {
        _bases.push_back(std::move(base));
    }

    // Register a conversion from the type to another.
    void registerConversion(Conversion conversion) {
        _conversions.push_back(std::move(conversion));
    }

    // Register a property for the type.
    void registerProperty(Property property) {
        _properties.push_back(std::move(property));
    }

//----------------------------  Private Interface  -----------------------------
private:
    // Allow creation of type information only through TypeInfo::instance.
    TypeInfo(TypeInfo const &) = delete;

    TypeInfo(std::type_info const &typeInfo)
    : _name(typeInfo.name()), _nameSet(false) { }

//-----------------------------  Private Members  ------------------------------
private:
    // Shortest name by which the type has been registered.
    std::string _name;
    bool _nameSet;
    // List of base classes registered for the type.
    std::vector<Base> _bases;
//    // List of constants registered for the type.
//    std::vector<Constant> _constants;
//    // List of constructors registered for the type.
//    std::vector<Constructor> _constructors;
    // List of conversions registered for the type.
    std::vector<Conversion> _conversions;
//    // List of extensions registered for the type.
//    std::vector<Extension> _extensions;
//    // List of functions registered for the type.
//    std::vector<Function> _functions;
    // List of properties registered for the type.
    std::vector<Property> _properties;
};

} }
//--                      End Namespace Reflect::Detail                       --
//------------------------------------------------------------------------------

#endif
