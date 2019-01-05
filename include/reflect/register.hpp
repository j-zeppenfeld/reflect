// Copyright (c) 2019 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#include "detail/accessor.h"
#include "detail/buffer.h"
#include "detail/type_info.h"

//------------------------------------------------------------------------------
//--                         Begin Namespace Reflect                          --
namespace Reflect {

//------------------------------------------------------------------------------
//--                              Class Register                              --
//------------------------------------------------------------------------------

//-------------------------------  Inheritance  --------------------------------

// Register type T_Base as a base class of type T.
template <typename T>
template <typename T_Base>
Register<T> &Register<T>::base() {
    static_assert(
        std::is_same<T_Base, typename std::decay<T_Base>::type>::value,
        "Base must be of unqualified type."
    );

    struct RegisterBase {
        // Upcast value to the base type.
        static void const *upcast(void const *value) {
            T_Base const *base = static_cast<T const *>(value);
            return base;
        }
    };

    // Register base class in the type information instance for T.
    Detail::TypeInfo::mutableInstance<T>()->registerBase(
        Detail::Base(
            Detail::TypeInfo::instance<T_Base>(),
            &RegisterBase::upcast
        )
    );

    return *this;
}

//--------------------------------  Conversion  --------------------------------

// Register a conversion from type T to type T_Target.
template <typename T>
template <typename T_Target>
Register<T> &Register<T>::conversion() {
    static_assert(
        std::is_same<T_Target, typename std::decay<T_Target>::type>::value,
        "Target must be of unqualified type."
    );

    struct RegisterConversion {
        // Retrieve value, which must be of type T, as the target type.
        static void *get(void const *value, Detail::Buffer<void> &buffer) {
            return buffer.construct<T_Target>(
                *static_cast<T const *>(value)
            );
        }

        // Set the accessed value in storage, which must be of the target type,
        // by copy-assigning value, which must be of type T.
        static bool set(Detail::Accessor const *accessor,
                        Detail::Storage &storage,
                        void const *value) {
            T_Target target = *static_cast<T const *>(value);
            return accessor->move(storage, &target);
        }

        // Set the accessed value in storage, which must be of the target type,
        // by move-assigning value, which must be of type T.
        static bool move(Detail::Accessor const *accessor,
                         Detail::Storage &storage,
                         void *value) {
            T_Target target = std::move(*static_cast<T *>(value));
            return accessor->move(storage, &target);
        }
    };

    // Register conversion in the type information instance for T.
    Detail::TypeInfo::mutableInstance<T>()->registerConversion(
        Detail::Conversion(
            Detail::TypeInfo::instance<T_Target>(),
            &RegisterConversion::get,
            &RegisterConversion::set,
            &RegisterConversion::move
        )
    );

    return *this;
}

template <typename T>
template <typename T_Target, typename T_Base, typename T_Ret>
Register<T> &Register<T>::conversion(T_Ret (T_Base::*method)() const) {
    static_assert(
        std::is_same<T_Target, typename std::decay<T_Target>::type>::value,
        "Target must be of unqualified type."
    );

    // Static storage for the conversion method pointer.
    // Multiple registrations of the same conversion are ignored, so storing
    // only the first conversion method is fine.
    static T_Ret (T_Base::*convert)() const = method;

    struct RegisterConversionMethod {
        // Retrieve value, which must be of type T, as the target type.
        static void *get(void const *value, Detail::Buffer<void> &buffer) {
            return buffer.construct<T_Target>(
                (static_cast<T const *>(value)->*convert)()
            );
        }

        // Set the accessed value in storage, which must be of the target type,
        // by copy-assigning value, which must be of type T.
        static bool set(Detail::Accessor const *accessor,
                        Detail::Storage &storage,
                        void const *value) {
            T_Target target = (static_cast<T const *>(value)->*convert)();
            return accessor->move(storage, &target);
        }

        // Set the accessed value in storage, which must be of the target type,
        // by move-assigning value, which must be of type T.
        static bool move(Detail::Accessor const *accessor,
                         Detail::Storage &storage,
                         void *value) {
            T_Target target = (static_cast<T const *>(value)->*convert)();
            return accessor->move(storage, &target);
        }
    };

    // Register conversion in the type information instance for T.
    Detail::TypeInfo::mutableInstance<T>()->registerConversion(
        Detail::Conversion(
            Detail::TypeInfo::instance<T_Target>(),
            &RegisterConversionMethod::get,
            &RegisterConversionMethod::set,
            &RegisterConversionMethod::move
        )
    );

    return *this;
}

template <typename T>
template <typename T_Target, typename T_Func>
Register<T> &Register<T>::conversion(T_Func &&function) {
    static_assert(
        std::is_same<T_Target, typename std::decay<T_Target>::type>::value,
        "Target must be of unqualified type."
    );

    // Static storage for the conversion function.
    // Multiple registrations of the same conversion are ignored, so storing
    // only the first conversion function is fine.
    static T_Func convert = std::forward<T_Func>(function);

    struct RegisterConversionFunction {
        // Retrieve value, which must be of type T, as the target type.
        static void *get(void const *value, Detail::Buffer<void> &buffer) {
            return buffer.construct<T_Target>(
                convert(*static_cast<T const *>(value))
            );
        }

        // Set the accessed value in storage, which must be of the target type,
        // by copy-assigning value, which must be of type T.
        static bool set(Detail::Accessor const *accessor,
                        Detail::Storage &storage,
                        void const *value) {
            T_Target target = convert(*static_cast<T const *>(value));
            return accessor->move(storage, &target);
        }

        // Set the accessed value in storage, which must be of the target type,
        // by move-assigning value, which must be of type T.
        static bool move(Detail::Accessor const *accessor,
                         Detail::Storage &storage,
                         void *value) {
            T_Target target = convert(std::move(*static_cast<T *>(value)));
            return accessor->move(storage, &target);
        }
    };

    // Register conversion in the type information instance for T.
    Detail::TypeInfo::mutableInstance<T>()->registerConversion(
        Detail::Conversion(
            Detail::TypeInfo::instance<T_Target>(),
            &RegisterConversionFunction::get,
            &RegisterConversionFunction::set,
            &RegisterConversionFunction::move
        )
    );

    return *this;
}

}
//--                          End Namespace Reflect                           --
//------------------------------------------------------------------------------
