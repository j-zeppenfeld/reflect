// Copyright (c) 2018 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#ifndef REFLECT_DETAIL_ITERATORRANGE_H
#define REFLECT_DETAIL_ITERATORRANGE_H

// std::move
#include <utility>

//------------------------------------------------------------------------------
//--                     Begin Namespace Reflect::Detail                      --
namespace Reflect { namespace Detail {

//------------------------------------------------------------------------------
//--                           Class IteratorRange                            --
//------------------------------------------------------------------------------
// Contains an iterator range for use in range based for loops.
template <typename T_It>
class IteratorRange {
public:
    IteratorRange(T_It begin, T_It end)
    : _begin(std::move(begin)), _end(std::move(end)) { }

    T_It const &begin() const { return _begin; }
    T_It const &end() const { return _end; }

private:
    T_It _begin;
    T_It _end;
};

} }
//--                      End Namespace Reflect::Detail                       --
//------------------------------------------------------------------------------

#endif
