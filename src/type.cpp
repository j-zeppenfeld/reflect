// Copyright (c) 2019 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#include "reflect/type.h"

//------------------------------------------------------------------------------
//--                         Begin Namespace Reflect                          --
namespace Reflect {

//------------------------------------------------------------------------------
//--                                Class Type                                --
//------------------------------------------------------------------------------

//--------------------------------  Operators  ---------------------------------

// Comparison operators.
bool operator==(Type const &lhs, Type const &rhs) {
    return (lhs._typeInfo == rhs._typeInfo &&
            lhs._constant == rhs._constant &&
            lhs._reference == rhs._reference);
}

bool operator!=(Type const &lhs, Type const &rhs) {
    return (lhs._typeInfo != rhs._typeInfo ||
            lhs._constant != rhs._constant ||
            lhs._reference != rhs._reference);
}

bool operator<(Type const &lhs, Type const &rhs) {
    if(lhs._typeInfo != rhs._typeInfo) {
        return (lhs._typeInfo < rhs._typeInfo);
    }
    if(lhs._constant != rhs._constant) {
        return rhs._constant;
    }
    if(lhs._reference != rhs._reference) {
        return rhs._reference;
    }
    return false;
}

bool operator<=(Type const &lhs, Type const &rhs) {
    if(lhs._typeInfo != rhs._typeInfo) {
        return (lhs._typeInfo < rhs._typeInfo);
    }
    if(lhs._constant != rhs._constant) {
        return rhs._constant;
    }
    if(lhs._reference != rhs._reference) {
        return rhs._reference;
    }
    return true;
}

bool operator>(Type const &lhs, Type const &rhs) {
    if(lhs._typeInfo != rhs._typeInfo) {
        return (lhs._typeInfo > rhs._typeInfo);
    }
    if(lhs._constant != rhs._constant) {
        return lhs._constant;
    }
    if(lhs._reference != rhs._reference) {
        return lhs._reference;
    }
    return false;
}

bool operator>=(Type const &lhs, Type const &rhs) {
    if(lhs._typeInfo != rhs._typeInfo) {
        return (lhs._typeInfo > rhs._typeInfo);
    }
    if(lhs._constant != rhs._constant) {
        return lhs._constant;
    }
    if(lhs._reference != rhs._reference) {
        return lhs._reference;
    }
    return true;
}

// Output streaming operator.
std::ostream &operator<<(std::ostream &os, Type const &type) {
    os << type._typeInfo->getName();
    if(type.isConstant()) os << " const";
    if(type.isReference()) os << " &";
    return os;
}

}
//--                          End Namespace Reflect                           --
//------------------------------------------------------------------------------
