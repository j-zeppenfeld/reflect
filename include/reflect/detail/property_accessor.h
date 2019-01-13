// Copyright (c) 2019 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#ifndef REFLECT_DETAIL_PROPERTYACCESSOR_H
#define REFLECT_DETAIL_PROPERTYACCESSOR_H

#include "accessor.h"
#include "traits.h"
#include "value_accessor.h"

//------------------------------------------------------------------------------
//--                     Begin Namespace Reflect::Detail                      --
namespace Reflect { namespace Detail {

//------------------------------------------------------------------------------
//--                          Class PropertyAccessor                          --
//------------------------------------------------------------------------------
template <typename T, typename T_Value, typename ...T_Accessors>
class PropertyAccessor;

//------------------------------------------------------------------------------
//--                       Class PropertyAccessor Base                        --
//------------------------------------------------------------------------------
template <>
class PropertyAccessor<void, void> : public Accessor {
//-------------------------------  Construction  -------------------------------
public:
    // Construct a reference to property of owner within storage.
    // Returns an accessor for the constructed value in storage.
    virtual Accessor const *construct(Storage &storage, void *owner) const = 0;

    // Construct a copy of the property value of owner within storage.
    // Returns an accessor for the constructed value in storage, or nullptr if
    // the property value cannot be accessed.
    virtual Accessor const *access(Storage &storage, void *owner) const {
        return nullptr;
    }

//----------------------------  Visitor Interface  -----------------------------
public:
    // Call visitor with a pointer to the value in storage.
    void *accept(Storage const &storage, Visitor &visitor) const override {
        return nullptr;
    }

//----------------------------  Internal Interface  ----------------------------
protected:
    PropertyAccessor(TypeInfo const *typeInfo, bool constant)
    : Accessor(typeInfo, constant, true) { }
};

//------------------------------------------------------------------------------
//--                     Class PropertyAccessor Type Base                     --
//------------------------------------------------------------------------------
template <typename T, typename T_Value>
class PropertyAccessor<T, T_Value> : public PropertyAccessor<void, void> {
public:
    using value_type = T_Value;

//-------------------------------  Construction  -------------------------------
public:
    // Construct a reference to property of owner within storage.
    Accessor const *construct(Storage &storage, void *owner) const override {
        storage.construct<T *>(static_cast<T *>(owner));
        return this;
    }

    // Construct a copy of value within storage.
    Accessor const *constructCopy(Storage &storage,
                                  Storage const &value) const override {
        return access(storage, (void *)value.get<T *>());
    }

    // Construct a reference to value within storage.
    Accessor const *constructReference(Storage &storage,
                                       Storage const &value,
                                       bool constant) const override {
        // TODO: Take constant parameter into account.
        return construct(storage, (void *)value.get<T *>());
    }

    // Destruct the value in storage.
    void destruct(Storage &storage) const override {
        storage.destruct<T *>();
    }

//----------------------------  Internal Interface  ----------------------------
protected:
    PropertyAccessor(bool constant)
    : PropertyAccessor<void, void>(TypeInfo::instance<T_Value>(), constant) { }
};

//------------------------------------------------------------------------------
//--                      Class PropertyAccessor<member>                      --
//------------------------------------------------------------------------------
template <typename T, typename T_Value,
          typename T_Owner, typename T_Member,
          typename ...T_Accessors>
class PropertyAccessor<T, T_Value, T_Member T_Owner::*, T_Accessors...>
: public PropertyAccessor<T, Demote<T_Member, T_Value>, T_Accessors...> {
public:
    using value_type = typename PropertyAccessor<
        T, Demote<T_Member, T_Value>, T_Accessors...
    >::value_type;

public:
    PropertyAccessor(bool constant, T_Member T_Owner::*member,
                     T_Accessors ...accessors)
    : PropertyAccessor<T, Demote<T_Member, T_Value>, T_Accessors...>
      (constant && std::is_const<T>::value,
       std::forward<T_Accessors>(accessors)...)
    , _member(member) { }

//-------------------------------  Construction  -------------------------------
public:
    // Construct a copy of the property value of owner within storage.
    Accessor const *access(Storage &storage, void *owner) const override {
        return ValueAccessor<T_Member>::construct(
            storage, static_cast<T *>(owner)->*_member
        );
    }

//----------------------------  Visitor Interface  -----------------------------
public:
    // Call visitor with a pointer to the value in storage.
    void *accept(Storage const &storage,
                 Accessor::Visitor &visitor) const override {
        return visitor.visit(
            const_cast<value_type *>(&(storage.get<T *>()->*_member)),
            std::is_const<T>::value, false
        );
    }

//-------------------------------  Value Access  -------------------------------
public:
    // Set the value in storage by copy-assigning the specified value.
    bool set(Storage &storage, void const *value) const override {
        return set(std::is_const<T>(), storage, value);
    }

    // Set the value in storage by move-assigning the specified value.
    bool move(Storage &storage, void *value) const override {
        return move(std::is_const<T>(), storage, value);
    }

private:
    // Implementation of set for mutable T.
    bool set(std::false_type, Storage &storage, void const *value) const {
        (storage.get<T *>()->*_member)
            = *static_cast<value_type const *>(value);
        return true;
    }
    // Implementation of set for constant T.
    bool set(std::true_type, Storage &storage, void const *value) const {
        return false;
    }

    // Implementation of move for mutable T.
    bool move(std::false_type, Storage &storage, void *value) const {
        (storage.get<T *>()->*_member)
            = std::move(*static_cast<value_type *>(value));
        return true;
    }
    // Implementation of move for constant T.
    bool move(std::true_type, Storage &storage, void *value) const {
        return false;
    }

//-----------------------------  Private Members  ------------------------------
private:
    T_Member T_Owner::*_member;
};

//------------------------------------------------------------------------------
//--                   Class PropertyAccessor<member const>                   --
//------------------------------------------------------------------------------
template <typename T, typename T_Value,
          typename T_Owner, typename T_Member,
          typename ...T_Accessors>
class PropertyAccessor<T, T_Value, T_Member const T_Owner::*, T_Accessors...>
: public PropertyAccessor<T, Demote<T_Member, T_Value>, T_Accessors...> {
public:
    using value_type = typename PropertyAccessor<
        T, Demote<T_Member, T_Value>, T_Accessors...
    >::value_type;

public:
    PropertyAccessor(bool constant, T_Member const T_Owner::*member,
                     T_Accessors ...accessors)
    : PropertyAccessor<T, Demote<T_Member, T_Value>, T_Accessors...>
      (constant, std::forward<T_Accessors>(accessors)...)
    , _member(member) { }

//-------------------------------  Construction  -------------------------------
public:
    // Construct a copy of the property value of owner within storage.
    Accessor const *access(Storage &storage, void *owner) const override {
        return ValueAccessor<T_Member>::construct(
            storage, static_cast<T *>(owner)->*_member
        );
    }

//----------------------------  Visitor Interface  -----------------------------
public:
    // Call visitor with a pointer to the value in storage.
    void *accept(Storage const &storage,
                 Accessor::Visitor &visitor) const override {
        return visitor.visit(
            const_cast<value_type *>(&(storage.get<T *>()->*_member)),
            true, false
        );
    }

//-----------------------------  Private Members  ------------------------------
private:
    T_Member const T_Owner::*_member;
};

//------------------------------------------------------------------------------
//--                   Class PropertyAccessor<value get()>                    --
//------------------------------------------------------------------------------
template <typename T, typename T_Value,
          typename T_Owner, typename T_Return,
          typename ...T_Accessors>
class PropertyAccessor<T, T_Value, T_Return (T_Owner::*)(), T_Accessors...>
: public PropertyAccessor<T, Demote<T_Return, T_Value>, T_Accessors...> {
public:
    using value_type = typename PropertyAccessor<
        T, Demote<T_Return, T_Value>, T_Accessors...
    >::value_type;

public:
    PropertyAccessor(bool constant, T_Return (T_Owner::*getFunc)(),
                     T_Accessors ...accessors)
    : PropertyAccessor<T, Demote<T_Return, T_Value>, T_Accessors...>
      (constant, std::forward<T_Accessors>(accessors)...)
    , _getFunc(getFunc) { }

//-------------------------------  Construction  -------------------------------
public:
    // Construct a copy of the property value of owner within storage.
    Accessor const *access(Storage &storage, void *owner) const override {
        return ValueAccessor<T_Return>::construct(
            storage, (static_cast<T *>(owner)->*_getFunc)()
        );
    }

//----------------------------  Visitor Interface  -----------------------------
public:
    // Call visitor with a pointer to the value in storage.
    void *accept(Storage const &storage,
                 Accessor::Visitor &visitor) const override {
        value_type &&value = (storage.get<T *>()->*_getFunc)();
        return visitor.visit(&value, false, true);
    }

//-----------------------------  Private Members  ------------------------------
private:
    T_Return (T_Owner::*_getFunc)();
};

template <typename T, typename T_Value,
          typename T_Owner, typename T_Return,
          typename ...T_Accessors>
class PropertyAccessor<T const, T_Value, T_Return (T_Owner::*)(),
                       T_Accessors...>
: public PropertyAccessor<T const, Preserve<T_Value, T_Return>,
                          T_Accessors...> {
public:
    using value_type = typename PropertyAccessor<
        T, Preserve<T_Value, T_Return>, T_Accessors...
    >::value_type;

public:
    PropertyAccessor(bool constant, T_Return (T_Owner::*)(),
                     T_Accessors ...accessors)
    : PropertyAccessor<T const, Preserve<T_Value, T_Return>, T_Accessors...>
      (constant, std::forward<T_Accessors>(accessors)...) { }
};

//------------------------------------------------------------------------------
//--                Class PropertyAccessor<value get() const>                 --
//------------------------------------------------------------------------------
template <typename T, typename T_Value,
          typename T_Owner, typename T_Return,
          typename ...T_Accessors>
class PropertyAccessor<T, T_Value, T_Return (T_Owner::*)() const,
                       T_Accessors...>
: public PropertyAccessor<T, Demote<T_Return, T_Value>, T_Accessors...> {
public:
    using value_type = typename PropertyAccessor<
        T, Demote<T_Return, T_Value>, T_Accessors...
    >::value_type;

public:
    PropertyAccessor(bool constant, T_Return (T_Owner::*getFunc)() const,
                     T_Accessors ...accessors)
    : PropertyAccessor<T, Demote<T_Return, T_Value>, T_Accessors...>
      (constant, std::forward<T_Accessors>(accessors)...)
    , _getFunc(getFunc) { }

//-------------------------------  Construction  -------------------------------
public:
    // Construct a copy of the property value of owner within storage.
    Accessor const *access(Storage &storage, void *owner) const override {
        return ValueAccessor<T_Return>::construct(
            storage, (static_cast<T *>(owner)->*_getFunc)()
        );
    }

//----------------------------  Visitor Interface  -----------------------------
public:
    // Call visitor with a pointer to the value in storage.
    void *accept(Storage const &storage,
                 Accessor::Visitor &visitor) const override {
        value_type &&value = (storage.get<T *>()->*_getFunc)();
        return visitor.visit(&value, false, true);
    }

//-----------------------------  Private Members  ------------------------------
private:
    T_Return (T_Owner::*_getFunc)() const;
};

//------------------------------------------------------------------------------
//--                   Class PropertyAccessor<value &get()>                   --
//------------------------------------------------------------------------------
template <typename T, typename T_Value,
          typename T_Owner, typename T_Return,
          typename ...T_Accessors>
class PropertyAccessor<T, T_Value, T_Return &(T_Owner::*)(), T_Accessors...>
: public PropertyAccessor<T, Demote<T_Return, T_Value>, T_Accessors...> {
public:
    using value_type = typename PropertyAccessor<
        T, Demote<T_Return, T_Value>, T_Accessors...
    >::value_type;

public:
    PropertyAccessor(bool constant, T_Return &(T_Owner::*getFunc)(),
                     T_Accessors ...accessors)
    : PropertyAccessor<T, Demote<T_Return, T_Value>, T_Accessors...>
      (false, std::forward<T_Accessors>(accessors)...)
    , _getFunc(getFunc) { }

//-------------------------------  Construction  -------------------------------
public:
    // Construct a copy of the property value of owner within storage.
    Accessor const *access(Storage &storage, void *owner) const override {
        return ValueAccessor<T_Return>::construct(
            storage, (static_cast<T *>(owner)->*_getFunc)()
        );
    }

//----------------------------  Visitor Interface  -----------------------------
public:
    // Call visitor with a pointer to the value in storage.
    void *accept(Storage const &storage,
                 Accessor::Visitor &visitor) const override {
        value_type &value = (storage.get<T *>()->*_getFunc)();
        return visitor.visit(&value, false, false);
    }

//-------------------------------  Value Access  -------------------------------
public:
    // Set the value in storage by copy-assigning the specified value.
    bool set(Storage &storage, void const *value) const override {
        (storage.get<T *>()->*_getFunc)()
            = *static_cast<value_type const *>(value);
        return true;
    }

    // Set the value in storage by move-assigning the specified value.
    bool move(Storage &storage, void *value) const override {
        (storage.get<T *>()->*_getFunc)()
            = std::move(*static_cast<value_type *>(value));
        return true;
    }

//-----------------------------  Private Members  ------------------------------
private:
    T_Return &(T_Owner::*_getFunc)();
};

template <typename T, typename T_Value,
          typename T_Owner, typename T_Return,
          typename ...T_Accessors>
class PropertyAccessor<T const, T_Value, T_Return &(T_Owner::*)(),
                       T_Accessors...>
: public PropertyAccessor<T const, Preserve<T_Value, T_Return>,
                          T_Accessors...> {
public:
    using value_type = typename PropertyAccessor<
        T, Preserve<T_Value, T_Return>, T_Accessors...
    >::value_type;

public:
    PropertyAccessor(bool constant, T_Return &(T_Owner::*)(),
                     T_Accessors ...accessors)
    : PropertyAccessor<T const, Preserve<T_Value, T_Return>, T_Accessors...>
      (constant, std::forward<T_Accessors>(accessors)...) { }
};

//------------------------------------------------------------------------------
//--                Class PropertyAccessor<value &get() const>                --
//------------------------------------------------------------------------------
template <typename T, typename T_Value,
          typename T_Owner, typename T_Return,
          typename ...T_Accessors>
class PropertyAccessor<T, T_Value, T_Return &(T_Owner::*)() const,
                       T_Accessors...>
: public PropertyAccessor<T, Demote<T_Return, T_Value>, T_Accessors...> {
public:
    using value_type = typename PropertyAccessor<
        T, Demote<T_Return, T_Value>, T_Accessors...
    >::value_type;

public:
    PropertyAccessor(bool constant, T_Return &(T_Owner::*getFunc)() const,
                     T_Accessors ...accessors)
    : PropertyAccessor<T, Demote<T_Return, T_Value>, T_Accessors...>
      (false, std::forward<T_Accessors>(accessors)...)
    , _getFunc(getFunc) { }

//-------------------------------  Construction  -------------------------------
public:
    // Construct a copy of the property value of owner within storage.
    Accessor const *access(Storage &storage, void *owner) const override {
        return ValueAccessor<T_Return>::construct(
            storage, (static_cast<T *>(owner)->*_getFunc)()
        );
    }

//----------------------------  Visitor Interface  -----------------------------
public:
    // Call visitor with a pointer to the value in storage.
    void *accept(Storage const &storage,
                 Accessor::Visitor &visitor) const override {
        value_type &value = (storage.get<T *>()->*_getFunc)();
        return visitor.visit(&value, false, false);
    }

//-------------------------------  Value Access  -------------------------------
public:
    // Set the value in storage by copy-assigning the specified value.
    bool set(Storage &storage, void const *value) const override {
        (storage.get<T *>()->*_getFunc)()
            = *static_cast<value_type const *>(value);
        return true;
    }

    // Set the value in storage by move-assigning the specified value.
    bool move(Storage &storage, void *value) const override {
        (storage.get<T *>()->*_getFunc)()
            = std::move(*static_cast<value_type *>(value));
        return true;
    }

//-----------------------------  Private Members  ------------------------------
private:
    T_Return &(T_Owner::*_getFunc)() const;
};

//------------------------------------------------------------------------------
//--                Class PropertyAccessor<value const &get()>                --
//------------------------------------------------------------------------------
template <typename T, typename T_Value,
          typename T_Owner, typename T_Return,
          typename ...T_Accessors>
class PropertyAccessor<T, T_Value, T_Return const &(T_Owner::*)(),
                       T_Accessors...>
: public PropertyAccessor<T, Demote<T_Return, T_Value>, T_Accessors...> {
public:
    using value_type = typename PropertyAccessor<
        T, Demote<T_Return, T_Value>, T_Accessors...
    >::value_type;

public:
    PropertyAccessor(bool constant, T_Return const &(T_Owner::*getFunc)(),
                     T_Accessors ...accessors)
    : PropertyAccessor<T, Demote<T_Return, T_Value>, T_Accessors...>
      (constant, std::forward<T_Accessors>(accessors)...)
    , _getFunc(getFunc) { }

//-------------------------------  Construction  -------------------------------
public:
    // Construct a copy of the property value of owner within storage.
    Accessor const *access(Storage &storage, void *owner) const override {
        return ValueAccessor<T_Return>::construct(
            storage, (static_cast<T *>(owner)->*_getFunc)()
        );
    }

//----------------------------  Visitor Interface  -----------------------------
public:
    // Call visitor with a pointer to the value in storage.
    void *accept(Storage const &storage,
                 Accessor::Visitor &visitor) const override {
        value_type const &value = (storage.get<T *>()->*_getFunc)();
        return visitor.visit(
            const_cast<value_type *>(&value),
            true, false
        );
    }

//-----------------------------  Private Members  ------------------------------
private:
    T_Return const &(T_Owner::*_getFunc)();
};

template <typename T, typename T_Value,
          typename T_Owner, typename T_Return,
          typename ...T_Accessors>
class PropertyAccessor<T const, T_Value, T_Return const &(T_Owner::*)(),
                       T_Accessors...>
: public PropertyAccessor<T const, Preserve<T_Value, T_Return>,
                          T_Accessors...> {
public:
    using value_type = typename PropertyAccessor<
        T, Preserve<T_Value, T_Return>, T_Accessors...
    >::value_type;

public:
    PropertyAccessor(bool constant, T_Return const &(T_Owner::*)(),
                     T_Accessors ...accessors)
    : PropertyAccessor<T const, Preserve<T_Value, T_Return>, T_Accessors...>
      (constant, std::forward<T_Accessors>(accessors)...) { }
};

//------------------------------------------------------------------------------
//--             Class PropertyAccessor<value const &get() const>             --
//------------------------------------------------------------------------------
template <typename T, typename T_Value,
          typename T_Owner, typename T_Return,
          typename ...T_Accessors>
class PropertyAccessor<T, T_Value, T_Return const &(T_Owner::*)() const,
                       T_Accessors...>
: public PropertyAccessor<T, Demote<T_Return, T_Value>, T_Accessors...> {
public:
    using value_type = typename PropertyAccessor<
        T, Demote<T_Return, T_Value>, T_Accessors...
    >::value_type;

public:
    PropertyAccessor(bool constant, T_Return const &(T_Owner::*getFunc)() const,
                     T_Accessors ...accessors)
    : PropertyAccessor<T, Demote<T_Return, T_Value>, T_Accessors...>
      (constant, std::forward<T_Accessors>(accessors)...)
    , _getFunc(getFunc) { }

//-------------------------------  Construction  -------------------------------
public:
    // Construct a copy of the property value of owner within storage.
    Accessor const *access(Storage &storage, void *owner) const override {
        return ValueAccessor<T_Return>::construct(
            storage, (static_cast<T *>(owner)->*_getFunc)()
        );
    }

//----------------------------  Visitor Interface  -----------------------------
public:
    // Call visitor with a pointer to the value in storage.
    void *accept(Storage const &storage,
                 Accessor::Visitor &visitor) const override {
        value_type const &value = (storage.get<T *>()->*_getFunc)();
        return visitor.visit(
            const_cast<value_type *>(&value),
            true, false
        );
    }

//-----------------------------  Private Members  ------------------------------
private:
    T_Return const &(T_Owner::*_getFunc)() const;
};

//------------------------------------------------------------------------------
//--                 Class PropertyAccessor<void set(value)>                  --
//------------------------------------------------------------------------------
template <typename T, typename T_Value,
          typename T_Owner, typename T_Param, typename T_Return,
          typename ...T_Accessors>
class PropertyAccessor<T, T_Value, T_Return (T_Owner::*)(T_Param),
                       T_Accessors...>
: public PropertyAccessor<T, Promote<T_Param, T_Value>, T_Accessors...> {
public:
    using value_type = typename PropertyAccessor<
        T, Promote<T_Param, T_Value>, T_Accessors...
    >::value_type;

public:
    PropertyAccessor(bool constant, T_Return (T_Owner::*setFunc)(T_Param),
                     T_Accessors ...accessors)
    : PropertyAccessor<T, Promote<T_Param, T_Value>, T_Accessors...>
      (false, std::forward<T_Accessors>(accessors)...)
    , _setFunc(setFunc) { }

//-------------------------------  Value Access  -------------------------------
public:
    // Set the value in storage by copy-assigning the specified value.
    bool set(Storage &storage, void const *value) const override {
        (storage.get<T *>()->*_setFunc)(
            *static_cast<value_type const *>(value)
        );
        return true;
    }

    // Set the value in storage by move-assigning the specified value.
    bool move(Storage &storage, void *value) const override {
        (storage.get<T *>()->*_setFunc)(
            std::move(*static_cast<value_type *>(value))
        );
        return true;
    }

//-----------------------------  Private Members  ------------------------------
private:
    T_Return (T_Owner::*_setFunc)(T_Param);
};

template <typename T, typename T_Value,
          typename T_Owner, typename T_Param, typename T_Return,
          typename ...T_Accessors>
class PropertyAccessor<T const, T_Value, T_Return (T_Owner::*)(T_Param),
                       T_Accessors...>
: public PropertyAccessor<T const, Preserve<T_Value, T_Param>, T_Accessors...> {
public:
    using value_type = typename PropertyAccessor<
        T, Preserve<T_Value, T_Param>, T_Accessors...
    >::value_type;

public:
    PropertyAccessor(bool constant, T_Return (T_Owner::*)(T_Param),
                     T_Accessors ...accessors)
    : PropertyAccessor<T const, Preserve<T_Value, T_Param>, T_Accessors...>
      (constant, std::forward<T_Accessors>(accessors)...) { }
};

//------------------------------------------------------------------------------
//--                Class PropertyAccessor<void set(value &)>                 --
//------------------------------------------------------------------------------
template <typename T, typename T_Value,
          typename T_Owner, typename T_Param, typename T_Return,
          typename ...T_Accessors>
class PropertyAccessor<T, T_Value, T_Return (T_Owner::*)(T_Param &),
                       T_Accessors...>
: public PropertyAccessor<T, Promote<T_Param, T_Value>, T_Accessors...> {
public:
    using value_type = typename PropertyAccessor<
        T, Promote<T_Param, T_Value>, T_Accessors...
    >::value_type;

public:
    PropertyAccessor(bool constant, T_Return (T_Owner::*setFunc)(T_Param &),
                     T_Accessors ...accessors)
    : PropertyAccessor<T, Promote<T_Param, T_Value>, T_Accessors...>
      (false, std::forward<T_Accessors>(accessors)...)
    , _setFunc(setFunc) { }

//-------------------------------  Value Access  -------------------------------
public:
    // Set the value in storage by move-assigning the specified value.
    bool move(Storage &storage, void *value) const override {
        (storage.get<T *>()->*_setFunc)(
            *static_cast<value_type *>(value)
        );
        return true;
    }

//-----------------------------  Private Members  ------------------------------
private:
    T_Return (T_Owner::*_setFunc)(T_Param &);
};

template <typename T, typename T_Value,
          typename T_Owner, typename T_Param, typename T_Return,
          typename ...T_Accessors>
class PropertyAccessor<T const, T_Value, T_Return (T_Owner::*)(T_Param &),
                       T_Accessors...>
: public PropertyAccessor<T const, Preserve<T_Value, T_Param>, T_Accessors...> {
public:
    using value_type = typename PropertyAccessor<
        T, Preserve<T_Value, T_Param>, T_Accessors...
    >::value_type;

public:
    PropertyAccessor(bool constant, T_Return (T_Owner::*)(T_Param &),
                     T_Accessors ...accessors)
    : PropertyAccessor<T const,Preserve<T_Value, T_Param>, T_Accessors...>
      (constant, std::forward<T_Accessors>(accessors)...) { }
};

//------------------------------------------------------------------------------
//--             Class PropertyAccessor<void set(value const &)>              --
//------------------------------------------------------------------------------
template <typename T, typename T_Value,
          typename T_Owner, typename T_Param, typename T_Return,
          typename ...T_Accessors>
class PropertyAccessor<T, T_Value, T_Return (T_Owner::*)(T_Param const &),
                       T_Accessors...>
: public PropertyAccessor<T, Promote<T_Param, T_Value>, T_Accessors...> {
public:
    using value_type = typename PropertyAccessor<
        T, Promote<T_Param, T_Value>, T_Accessors...
    >::value_type;

public:
    PropertyAccessor(bool constant,
                     T_Return (T_Owner::*setFunc)(T_Param const &),
                     T_Accessors ...accessors)
    : PropertyAccessor<T, Promote<T_Param, T_Value>, T_Accessors...>
      (false, std::forward<T_Accessors>(accessors)...)
    , _setFunc(setFunc) { }

//-------------------------------  Value Access  -------------------------------
public:
    // Set the value in storage by copy-assigning the specified value.
    bool set(Storage &storage, void const *value) const override {
        (storage.get<T *>()->*_setFunc)(
            *static_cast<value_type const *>(value)
        );
        return true;
    }

//-----------------------------  Private Members  ------------------------------
private:
    T_Return (T_Owner::*_setFunc)(T_Param const &);
};

template <typename T, typename T_Value,
          typename T_Owner, typename T_Param, typename T_Return,
          typename ...T_Accessors>
class PropertyAccessor<T const, T_Value, T_Return (T_Owner::*)(T_Param const &),
                       T_Accessors...>
: public PropertyAccessor<T const, Preserve<T_Value, T_Param>, T_Accessors...> {
public:
    using value_type = typename PropertyAccessor<
        T, Preserve<T_Value, T_Param>, T_Accessors...
    >::value_type;

public:
    PropertyAccessor(bool constant, T_Return (T_Owner::*)(T_Param const &),
                     T_Accessors ...accessors)
    : PropertyAccessor<T const, Preserve<T_Value, T_Param>, T_Accessors...>
      (constant, std::forward<T_Accessors>(accessors)...) { }
};

} }
//--                      End Namespace Reflect::Detail                       --
//------------------------------------------------------------------------------

#endif
