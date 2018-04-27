// Copyright (c) 2018 Johannes Zeppenfeld
// Distributed under the MIT license, see LICENSE file in repository root.

#ifndef REFLECT_DETAIL_ACCESSOR_H
#define REFLECT_DETAIL_ACCESSOR_H

//------------------------------------------------------------------------------
//--                     Begin Namespace Reflect::Detail                      --
namespace Reflect { namespace Detail {

// Uses.
class Storage;

//------------------------------------------------------------------------------
//--                              Class Accessor                              --
//------------------------------------------------------------------------------
// Provides type-erased access to a storage.
class Accessor {
public:
    // Allocate a copy of source within target.
    // Source and target must both be of the accessed type.
    // Returns an accessor for the allocated value in target.
    virtual Accessor const *allocateCopy(Storage const &source,
                                         Storage &target) const;

    // Allocate a moved copy of source within target.
    // Source and target must both be of the accessed type.
    // Returns an accessor for the allocated value in target.
    virtual Accessor const *allocateMove(Storage &source,
                                         Storage &target) const {
        return allocateCopy(source, target);
    }

    // Allocate a reference to source within target.
    // Source and target must both be of the accessed type.
    // Returns an accessor for the allocated value in target.
    virtual Accessor const *allocateReference(Storage const &source,
                                              Storage &target,
                                              bool constant) const;

    // Deallocate the value in storage, which must be of the accessed type.
    virtual void deallocate(Storage &storage) const;
};

} }
//--                      End Namespace Reflect::Detail                       --
//------------------------------------------------------------------------------

#endif
