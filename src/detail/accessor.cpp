// Copyright (c) 2019 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#include "reflect/detail/accessor.h"

#include "reflect/detail/base.h"
#include "reflect/detail/buffer.h"
#include "reflect/detail/conversion.h"
#include "reflect/detail/type_info.h"

#include <stdexcept>

//------------------------------------------------------------------------------
//--                     Begin Namespace Reflect::Detail                      --
namespace Reflect { namespace Detail {

//------------------------------------------------------------------------------
//--                              Class Accessor                              --
//------------------------------------------------------------------------------

//-------------------------------  Value Access  -------------------------------

namespace {
    // Visitor that converts the accessed value to another type.
    class ConversionVisitor : public Accessor::Visitor {
    public:
        ConversionVisitor(TypeInfo const *targetTypeInfo,
                          Buffer<void> *buffer)
        : _targetTypeInfo(targetTypeInfo)
        , _buffer(buffer) { }

        // Convert value from typeInfo to _targetTypeInfo.
        // If referable is true, a direct reference to value may be returned.
        // If movable is true, value references a temporary that may be moved.
        void *convert(TypeInfo const *typeInfo, void *value,
                      bool referable, bool movable) {
            // No conversion is necessary if typeInfo matches the target type.
            if(typeInfo == _targetTypeInfo) {
                // Return a reference to value if possible.
                if(referable) {
                    return value;
                // Otherwise, try to move or copy value into the target buffer.
                } else if(_buffer) {
                    void *copy = movable ? _buffer->constructMove(value)
                                         : _buffer->constructCopy(value);
                    if(copy) return copy;
                }
            }

            // If a target buffer is available, look for a registered conversion
            // from typeInfo to the target type.
            if(_buffer) {
                for(auto &&conversion : typeInfo->getConversions()) {
                    if(conversion.getTypeInfo() == _targetTypeInfo) {
                        return conversion.get(value, *_buffer);
                    }
                }
            }

            // Recursively check base classes.
            for(auto &&base : typeInfo->getBases()) {
                void *converted = convert(base.getTypeInfo(),
                                          base.upcast(value),
                                          referable,
                                          movable);
                if(converted) return converted;
            }

            // No conversion found.
            return nullptr;
        }

    private:
        // Type information of the conversion target type.
        TypeInfo const *_targetTypeInfo;
        // Optional buffer into which an instance of the target type can be
        // constructed.
        Buffer<void> *_buffer;
    };

    // Convert value from the type associated with typeInfo and copy-assign it
    // to the accessed storage.
    bool convertAndSet(Accessor const *accessor, Storage &storage,
                       TypeInfo const *typeInfo, void const *value) {
        // Look for a registered conversion from typeInfo to the accessed type.
        for(auto &&conversion : typeInfo->getConversions()) {
            if(conversion.getTypeInfo() == accessor->getTypeInfo()) {
                if(conversion.set(accessor, storage, value)) {
                    return true;
                }
            }
        }

        // Recursively check base classes.
        for(auto &&base : typeInfo->getBases()) {
            void const *upcast = base.upcast(value);
            if(base.getTypeInfo() == accessor->getTypeInfo()) {
                return accessor->set(storage, upcast);
            }
            if(convertAndSet(accessor, storage, base.getTypeInfo(), upcast)) {
                return true;
            }
        }

        return false;
    }

    // Convert value from the type associated with typeInfo and move-assign it
    // to the accessed storage.
    bool convertAndMove(Accessor const *accessor, Storage &storage,
                        TypeInfo const *typeInfo, void *value) {
        // Look for a registered conversion from typeInfo to the accessed type.
        for(auto &&conversion : typeInfo->getConversions()) {
            if(conversion.getTypeInfo() == accessor->getTypeInfo()) {
                if(conversion.move(accessor, storage, value)) {
                    return true;
                }
            }
        }

        // Recursively check base classes.
        for(auto &&base : typeInfo->getBases()) {
            void *upcast = base.upcast(value);
            if(base.getTypeInfo() == accessor->getTypeInfo()) {
                return accessor->move(storage, upcast);
            }
            if(convertAndMove(accessor, storage, base.getTypeInfo(), upcast)) {
                return true;
            }
        }

        return false;
    }
}

// Retrieve the value in storage as the type associated with typeInfo.
void *Accessor::getAs(Storage const &storage,
                      TypeInfo const *typeInfo,
                      Buffer<void> *buffer) const {
    // Create visitor to retrieve and convert value from storage.
    class Visitor : public ConversionVisitor {
    public:
        Visitor(TypeInfo const *sourceTypeInfo,
                TypeInfo const *targetTypeInfo,
                Buffer<void> *buffer)
        : ConversionVisitor(targetTypeInfo, buffer)
        , _sourceTypeInfo(sourceTypeInfo) { }

        void *visit(void *value, bool constant, bool temporary) override {
            return convert(_sourceTypeInfo, value,
                           !constant && !temporary, !constant && temporary);
        }

    private:
        TypeInfo const *_sourceTypeInfo;
    } visitor(_typeInfo, typeInfo, buffer);

    void *result = accept(storage, visitor);
    if(!result) {
        throw std::runtime_error(
            "Could not retrieve type '" + _typeInfo->getName()
            + (_constant ? " const" : "") + (_reference ? " &" : "")
            + "' as type '" + typeInfo->getName() + (buffer ? "" : " &")
            + "'."
        );
    }
    return result;
}

// Retrieve the value in storage as the constant type associated with typeInfo.
void const *Accessor::getAsConst(Storage const &storage,
                                 TypeInfo const *typeInfo,
                                 Buffer<void> *buffer) const {
    // Create visitor to retrieve and convert value from storage.
    class Visitor : public ConversionVisitor {
    public:
        Visitor(TypeInfo const *sourceTypeInfo,
                TypeInfo const *targetTypeInfo,
                Buffer<void> *buffer)
        : ConversionVisitor(targetTypeInfo, buffer)
        , _sourceTypeInfo(sourceTypeInfo) { }

        void *visit(void *value, bool constant, bool temporary) override {
            return convert(_sourceTypeInfo, value,
                           !temporary, !constant && temporary);
        }

    private:
        TypeInfo const *_sourceTypeInfo;
    } visitor(_typeInfo, typeInfo, buffer);

    void const *result = accept(storage, visitor);
    if(!result) {
        throw std::runtime_error(
            "Could not retrieve type '" + _typeInfo->getName()
            + " const" + (_reference ? " &" : "")
            + "' as type '" + typeInfo->getName() + (buffer ? "" : " const &")
            + "'."
        );
    }
    return result;
}

// Set the value in storage, which must be of the accessed type, by
// copy-assigning the specified value of the type associated with typeInfo.
void Accessor::setAs(Storage &storage,
                     TypeInfo const *typeInfo,
                     void const *value) const {
    if(typeInfo == _typeInfo) {
        if(set(storage, value)) return;
    }
    if(!convertAndSet(this, storage, typeInfo, value)) {
        throw std::runtime_error(
            "Could not set type '" + _typeInfo->getName()
            + (_constant ? " const" : "") + (_reference ? " &" : "")
            + "' from type '" + typeInfo->getName() + "'."
        );
    }
}

// Set the value in storage, which must be of the accessed type, by
// copy-assigning the value accessed by accessor.
void Accessor::setAs(Storage &storage,
                     Accessor const *accessor,
                     Storage const &value) const {
    // Create visitor to retrieve accessed value.
    class Visitor : public Accessor::Visitor {
    public:
        Visitor(Accessor const *accessor,
                Storage &storage,
                TypeInfo const *typeInfo)
        : _accessor(accessor)
        , _storage(storage)
        , _typeInfo(typeInfo) { }

        void *visit(void *value, bool constant, bool temporary) override {
            if(temporary && !constant) {
                _accessor->moveAs(_storage, _typeInfo, value);
            } else {
                _accessor->setAs(_storage, _typeInfo, value);
            }
            return nullptr;
        }

    private:
        Accessor const *_accessor;
        Storage &_storage;
        TypeInfo const *_typeInfo;
    } visitor(this, storage, accessor->getTypeInfo());

    accessor->accept(value, visitor);
}

// Set the value in storage, which must be of the accessed type, by
// move-assigning the specified value of the type associated with typeInfo.
void Accessor::moveAs(Storage &storage,
                      TypeInfo const *typeInfo,
                      void *value) const {
    if(typeInfo == _typeInfo) {
        if(move(storage, value)) return;
    }
    if(!convertAndMove(this, storage, typeInfo, value)) {
        throw std::runtime_error(
            "Could not set type '" + _typeInfo->getName()
            + (_constant ? " const" : "") + (_reference ? " &" : "")
            + "' from type '" + typeInfo->getName() + "'."
        );
    }
}

// Set the value in storage, which must be of the accessed type, by
// move-assigning the value accessed by accessor.
void Accessor::moveAs(Storage &storage,
                      Accessor const *accessor,
                      Storage &value) const {
    // Create visitor to retrieve accessed value.
    class Visitor : public Accessor::Visitor {
    public:
        Visitor(Accessor const *accessor,
                Storage &storage,
                TypeInfo const *typeInfo)
        : _accessor(accessor)
        , _storage(storage)
        , _typeInfo(typeInfo) { }

        void *visit(void *value, bool constant, bool temporary) override {
            if(constant) {
                _accessor->setAs(_storage, _typeInfo, value);
            } else {
                _accessor->moveAs(_storage, _typeInfo, value);
            }
            return nullptr;
        }

    private:
        Accessor const *_accessor;
        Storage &_storage;
        TypeInfo const *_typeInfo;
    } visitor(this, storage, accessor->getTypeInfo());

    accessor->accept(value, visitor);
}

} }
//--                      End Namespace Reflect::Detail                       --
//------------------------------------------------------------------------------
