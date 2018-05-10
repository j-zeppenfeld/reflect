// Copyright (c) 2018 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#include "reflect/detail/accessor.h"

#include "reflect/detail/base.h"
#include "reflect/detail/conversion.h"
#include "reflect/detail/type_info.h"

#include <stdexcept>

//------------------------------------------------------------------------------
//--                     Begin Namespace Reflect::Detail                      --
namespace Reflect { namespace Detail {

//------------------------------------------------------------------------------
//--                              Class Accessor                              --
//------------------------------------------------------------------------------

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
        void *convert(TypeInfo const *typeInfo, void *value, bool referable) {
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
                void *upcast = base.upcast(value);
                if(base.getTypeInfo() == _targetTypeInfo && referable) {
                    return upcast;
                }
                void *converted = convert(base.getTypeInfo(),
                                          upcast,
                                          referable);
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
    void *result;
    if(typeInfo == _typeInfo) {
        // Retrieve value directly from storage.
        auto value = get(storage, buffer);
        if(value._constant) {
            result = nullptr;
        } else {
            result = value._value;
        }
    } else {
        // Create visitor to retrieve and convert value from storage.
        class Visitor : public ConversionVisitor {
        public:
            Visitor(TypeInfo const *sourceTypeInfo,
                    TypeInfo const *targetTypeInfo,
                    Buffer<void> *buffer)
            : ConversionVisitor(targetTypeInfo, buffer)
            , _sourceTypeInfo(sourceTypeInfo) { }

            void *visit(void *value, bool constant, bool temporary) override {
                return convert(_sourceTypeInfo, value, !constant && !temporary);
            }

        private:
            TypeInfo const *_sourceTypeInfo;
        } visitor(_typeInfo, typeInfo, buffer);

        result = accept(storage, visitor);
    }

    if(!result) {
        throw std::runtime_error(
            "Accessing value as incompatible type."
        );
    }
    return result;
}

// Retrieve the value in storage as the constant type associated with typeInfo.
void const *Accessor::getAsConst(Storage const &storage,
                                 TypeInfo const *typeInfo,
                                 Buffer<void> *buffer) const {
    void const *result;
    if(typeInfo == _typeInfo) {
        // Retrieve value directly from storage.
        result = get(storage, buffer)._value;
    } else {
        // Create visitor to retrieve and convert value from storage.
        class Visitor : public ConversionVisitor {
        public:
            Visitor(TypeInfo const *sourceTypeInfo,
                    TypeInfo const *targetTypeInfo,
                    Buffer<void> *buffer)
            : ConversionVisitor(targetTypeInfo, buffer)
            , _sourceTypeInfo(sourceTypeInfo) { }

            void *visit(void *value, bool constant, bool temporary) override {
                return convert(_sourceTypeInfo, value, !temporary);
            }

        private:
            TypeInfo const *_sourceTypeInfo;
        } visitor(_typeInfo, typeInfo, buffer);

        result = accept(storage, visitor);
    }

    if(!result) {
        throw std::runtime_error(
            "Accessing value as incompatible type."
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
            "Setting value from incompatible type."
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
            _accessor->setAs(_storage, _typeInfo, value);
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
            "Setting value from incompatible type."
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