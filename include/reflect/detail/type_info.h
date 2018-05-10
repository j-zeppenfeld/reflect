// Copyright (c) 2018 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#ifndef REFLECT_DETAIL_TYPEINFO_H
#define REFLECT_DETAIL_TYPEINFO_H

#include "base.h"
//#include "constant.h"
//#include "constructor.h"
#include "conversion.h"
//#include "extension.h"
//#include "function.h"
//#include "property.h"

#include "iterator_range.h"

#include <string>
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
        static_assert(
            std::is_same<T, typename std::decay<T>::type>::value,
            "Internal error: Type information instance must be decayed."
        );

        static TypeInfo typeInfo;
        return &typeInfo;
    }

//-----------------------------  Public Interface  -----------------------------
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

//----------------------------  Private Interface  -----------------------------
private:
    // Allow creation of type information only through TypeInfo::instance.
    TypeInfo() { }
    TypeInfo(TypeInfo const &) = delete;

public:
    // Register a base class for the type.
    void registerBase(Base base) {
        _bases.push_back(std::move(base));
    }

    // Register a conversion from the type to another.
    void registerConversion(Conversion conversion) {
        _conversions.push_back(std::move(conversion));
    }

//---------------------------------  Members  ----------------------------------
private:
    // Shortest name by which the type was registered.
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
//    // List of properties registered for the type.
//    std::vector<Property> _properties;
};

} }
//--                      End Namespace Reflect::Detail                       --
//------------------------------------------------------------------------------

#endif
