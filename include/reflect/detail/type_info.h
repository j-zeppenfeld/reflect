// Copyright (c) 2018 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#ifndef REFLECT_DETAIL_TYPEINFO_H
#define REFLECT_DETAIL_TYPEINFO_H

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

//----------------------------  Internal Interface  ----------------------------
private:
    // Allow creation of type information only through TypeInfo::instance.
    TypeInfo() { }
    TypeInfo(TypeInfo const &) = delete;
};

} }
//--                      End Namespace Reflect::Detail                       --
//------------------------------------------------------------------------------

#endif
