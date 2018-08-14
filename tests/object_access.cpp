// Copyright (c) 2018 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#include "common/catch.hpp"
#include "common/classes.h"

#include "reflect/object.h"

//------------------------------------------------------------------------------
//--                                Test Cases                                --
//------------------------------------------------------------------------------

TEST_CASE("Get object value by value",
          "[object][access]") {
    SECTION("from an object owning its value.") {
        Reflect::Object<Base> objBase;
        Reflect::Object<Base> objUpcast = Derived();
        Reflect::Object<Derived> objDerived;
        Count<All>::clear();

        REQUIRE(objBase.getType() == Reflect::getType<Base>());
        REQUIRE(objUpcast.getType() == Reflect::getType<Derived>());
        REQUIRE(objDerived.getType() == Reflect::getType<Derived>());

        Base valueBase = objBase.get<Base>();
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(valueBase.getFrom() == &objBase.get<Base const &>());

        Base valueUpcast = objUpcast.get<Base>();
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(valueUpcast.getFrom() == &objUpcast.get<Base const &>());

        Base valueDerived = objDerived.get<Base>();
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(valueDerived.getFrom() == &objDerived.get<Base const &>());
    }

    SECTION("from an object referencing a mutable value.") {
        Base base;
        Derived derived;
        Reflect::Object<Base> objBase = std::ref(base);
        Reflect::Object<Base> objUpcast = std::ref(derived);
        Reflect::Object<Derived> objDerived = std::ref(derived);
        Count<All>::clear();

        REQUIRE(objBase.getType() == Reflect::getType<Base &>());
        REQUIRE(objUpcast.getType() == Reflect::getType<Derived &>());
        REQUIRE(objDerived.getType() == Reflect::getType<Derived &>());

        Base valueBase = objBase.get<Base>();
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(valueBase.getFrom() == &base);

        Base valueUpcast = objUpcast.get<Base>();
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(valueUpcast.getFrom() == &derived);

        Base valueDerived = objDerived.get<Base>();
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(valueDerived.getFrom() == &derived);
    }

    SECTION("from an object referencing a constant value.") {
        Base const base;
        Derived const derived;
        Reflect::Object<Base> objBase = std::ref(base);
        Reflect::Object<Base> objUpcast = std::ref(derived);
        Reflect::Object<Derived> objDerived = std::ref(derived);
        Count<All>::clear();

        REQUIRE(objBase.getType() == Reflect::getType<Base const &>());
        REQUIRE(objUpcast.getType() == Reflect::getType<Derived const &>());
        REQUIRE(objDerived.getType() == Reflect::getType<Derived const &>());

        Base valueBase = objBase.get<Base>();
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(valueBase.getFrom() == &base);

        Base valueUpcast = objUpcast.get<Base>();
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(valueUpcast.getFrom() == &derived);

        Base valueDerived = objDerived.get<Base>();
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(valueDerived.getFrom() == &derived);
    }

    REQUIRE(Count<All>::clear());
}

TEST_CASE("Get object value by mutable reference",
          "[object][access]") {
    SECTION("from an object owning its value.") {
        Reflect::Object<Base> objBase;
        Reflect::Object<Base> objUpcast = Derived();
        Reflect::Object<Derived> objDerived;
        Count<All>::clear();

        REQUIRE(objBase.getType() == Reflect::getType<Base>());
        REQUIRE(objUpcast.getType() == Reflect::getType<Derived>());
        REQUIRE(objDerived.getType() == Reflect::getType<Derived>());

        Base &valueBase = objBase.get<Base &>();
        REQUIRE(Count<Base>::constructed() == 0);
        REQUIRE(&valueBase == &objBase.get<Base const &>());

        Base &valueUpcast = objUpcast.get<Base &>();
        REQUIRE(Count<Base>::constructed() == 0);
        REQUIRE(&valueUpcast == &objUpcast.get<Base const &>());

        Base &valueDerived = objDerived.get<Base &>();
        REQUIRE(Count<Base>::constructed() == 0);
        REQUIRE(&valueDerived == &objDerived.get<Base const &>());
    }

    SECTION("from an object referencing a mutable value.") {
        Base base;
        Derived derived;
        Reflect::Object<Base> objBase = std::ref(base);
        Reflect::Object<Base> objUpcast = std::ref(derived);
        Reflect::Object<Derived> objDerived = std::ref(derived);
        Count<All>::clear();

        REQUIRE(objBase.getType() == Reflect::getType<Base &>());
        REQUIRE(objUpcast.getType() == Reflect::getType<Derived &>());
        REQUIRE(objDerived.getType() == Reflect::getType<Derived &>());

        Base &valueBase = objBase.get<Base &>();
        REQUIRE(Count<Base>::constructed() == 0);
        REQUIRE(&valueBase == &base);

        Base &valueUpcast = objUpcast.get<Base &>();
        REQUIRE(Count<Base>::constructed() == 0);
        REQUIRE(&valueUpcast == &derived);

        Base &valueDerived = objDerived.get<Base &>();
        REQUIRE(Count<Base>::constructed() == 0);
        REQUIRE(&valueDerived == &derived);
    }

    SECTION("from an object referencing a constant value.") {
        Base const base;
        Derived const derived;
        Reflect::Object<Base> objBase = std::ref(base);
        Reflect::Object<Base> objUpcast = std::ref(derived);
        Reflect::Object<Derived> objDerived = std::ref(derived);
        Count<All>::clear();

        REQUIRE(objBase.getType() == Reflect::getType<Base const &>());
        REQUIRE(objUpcast.getType() == Reflect::getType<Derived const &>());
        REQUIRE(objDerived.getType() == Reflect::getType<Derived const &>());

        REQUIRE_THROWS_AS(objBase.get<Base &>(), std::runtime_error);
        REQUIRE_THROWS_AS(objUpcast.get<Base &>(), std::runtime_error);
        REQUIRE_THROWS_AS(objDerived.get<Base &>(), std::runtime_error);
    }

    REQUIRE(Count<All>::clear());
}

TEST_CASE("Get object value by constant reference",
          "[object][access]") {
    SECTION("from an object owning its value.") {
        Reflect::Object<Base> objBase;
        Reflect::Object<Base> objUpcast = Derived();
        Reflect::Object<Derived> objDerived;
        Count<All>::clear();

        REQUIRE(objBase.getType() == Reflect::getType<Base>());
        REQUIRE(objUpcast.getType() == Reflect::getType<Derived>());
        REQUIRE(objDerived.getType() == Reflect::getType<Derived>());

        Base const &valueBase = objBase.get<Base const &>();
        REQUIRE(Count<Base>::constructed() == 0);
        REQUIRE(&valueBase == &objBase.get<Base const &>());

        Base const &valueUpcast = objUpcast.get<Base const &>();
        REQUIRE(Count<Base>::constructed() == 0);
        REQUIRE(&valueUpcast == &objUpcast.get<Base const &>());

        Base const &valueDerived = objDerived.get<Base const &>();
        REQUIRE(Count<Base>::constructed() == 0);
        REQUIRE(&valueDerived == &objDerived.get<Base const &>());
    }

    SECTION("from an object referencing a mutable value.") {
        Base base;
        Derived derived;
        Reflect::Object<Base> objBase = std::ref(base);
        Reflect::Object<Base> objUpcast = std::ref(derived);
        Reflect::Object<Derived> objDerived = std::ref(derived);
        Count<All>::clear();

        REQUIRE(objBase.getType() == Reflect::getType<Base &>());
        REQUIRE(objUpcast.getType() == Reflect::getType<Derived &>());
        REQUIRE(objDerived.getType() == Reflect::getType<Derived &>());

        Base const &valueBase = objBase.get<Base const &>();
        REQUIRE(Count<Base>::constructed() == 0);
        REQUIRE(&valueBase == &base);

        Base const &valueUpcast = objUpcast.get<Base const &>();
        REQUIRE(Count<Base>::constructed() == 0);
        REQUIRE(&valueUpcast == &derived);

        Base const &valueDerived = objDerived.get<Base const &>();
        REQUIRE(Count<Base>::constructed() == 0);
        REQUIRE(&valueDerived == &derived);
    }

    SECTION("from an object referencing a constant value.") {
        Base const base;
        Derived const derived;
        Reflect::Object<Base> objBase = std::ref(base);
        Reflect::Object<Base> objUpcast = std::ref(derived);
        Reflect::Object<Derived> objDerived = std::ref(derived);
        Count<All>::clear();

        REQUIRE(objBase.getType() == Reflect::getType<Base const &>());
        REQUIRE(objUpcast.getType() == Reflect::getType<Derived const &>());
        REQUIRE(objDerived.getType() == Reflect::getType<Derived const &>());

        Base const &valueBase = objBase.get<Base const &>();
        REQUIRE(Count<Base>::constructed() == 0);
        REQUIRE(&valueBase == &base);

        Base const &valueUpcast = objUpcast.get<Base const &>();
        REQUIRE(Count<Base>::constructed() == 0);
        REQUIRE(&valueUpcast == &derived);

        Base const &valueDerived = objDerived.get<Base const &>();
        REQUIRE(Count<Base>::constructed() == 0);
        REQUIRE(&valueDerived == &derived);
    }

    REQUIRE(Count<All>::clear());
}

TEST_CASE("Set object value by copying a value",
          "[object][access]") {
    Base valueBase;
    Derived valueDerived;

    SECTION("to an object owning its value.") {
        Reflect::Object<Base> objBase;
        Reflect::Object<Base> objUpcast = Derived();
        Count<All>::clear();

        REQUIRE(objBase.getType() == Reflect::getType<Base>());
        REQUIRE(objUpcast.getType() == Reflect::getType<Derived>());

        objBase.set(valueBase);
        REQUIRE(Count<Base>::copyAssigned() == 1);
        REQUIRE(objBase.get().getFrom() == &valueBase);

        objBase.set(valueDerived);
        REQUIRE(Count<Base>::copyAssigned() == 1);
        REQUIRE(objBase.get().getFrom() == &valueDerived);

        REQUIRE_THROWS_AS(objUpcast.set(valueBase), std::runtime_error);

        objUpcast.set(valueDerived);
        REQUIRE(Count<Base>::copyAssigned() == 1);
        REQUIRE(Count<Derived>::copyAssigned() == 1);
        REQUIRE(objUpcast.get().getFrom() == &valueDerived);
    }

    SECTION("to an object referencing a mutable value.") {
        Base base;
        Derived derived;
        Reflect::Object<Base> objBase = std::ref(base);
        Reflect::Object<Base> objUpcast = std::ref(derived);
        Count<All>::clear();

        REQUIRE(objBase.getType() == Reflect::getType<Base &>());
        REQUIRE(objUpcast.getType() == Reflect::getType<Derived &>());

        objBase.set(valueBase);
        REQUIRE(Count<Base>::copyAssigned() == 1);
        REQUIRE(objBase.get().getFrom() == &valueBase);

        objBase.set(valueDerived);
        REQUIRE(Count<Base>::copyAssigned() == 1);
        REQUIRE(objBase.get().getFrom() == &valueDerived);

        REQUIRE_THROWS_AS(objUpcast.set(valueBase), std::runtime_error);

        objUpcast.set(valueDerived);
        REQUIRE(Count<Base>::copyAssigned() == 1);
        REQUIRE(Count<Derived>::copyAssigned() == 1);
        REQUIRE(objUpcast.get().getFrom() == &valueDerived);
    }

    SECTION("to an object referencing a constant value.") {
        Base const base;
        Derived const derived;
        Reflect::Object<Base> objBase = std::ref(base);
        Reflect::Object<Base> objUpcast = std::ref(derived);
        Count<All>::clear();

        REQUIRE(objBase.getType() == Reflect::getType<Base const &>());
        REQUIRE(objUpcast.getType() == Reflect::getType<Derived const &>());

        REQUIRE_THROWS_AS(objBase.set(valueBase), std::runtime_error);
        REQUIRE_THROWS_AS(objBase.set(valueDerived), std::runtime_error);
        REQUIRE_THROWS_AS(objUpcast.set(valueBase), std::runtime_error);
        REQUIRE_THROWS_AS(objUpcast.set(valueDerived), std::runtime_error);
    }

    REQUIRE(Count<All>::clear());
}

TEST_CASE("Set object value by moving a value",
          "[object][access]") {
    Base valueBase;
    Derived valueDerived;

    SECTION("to an object owning its value.") {
        Reflect::Object<Base> objBase;
        Reflect::Object<Base> objUpcast = Derived();
        Count<All>::clear();

        REQUIRE(objBase.getType() == Reflect::getType<Base>());
        REQUIRE(objUpcast.getType() == Reflect::getType<Derived>());

        objBase.set(std::move(valueBase));
        REQUIRE(Count<Base>::moveAssigned() == 1);
        REQUIRE(objBase.get().getFrom() == &valueBase);

        objBase.set(std::move(valueDerived));
        REQUIRE(Count<Base>::moveAssigned() == 1);
        REQUIRE(objBase.get().getFrom() == &valueDerived);

        REQUIRE_THROWS_AS(
            objUpcast.set(std::move(valueBase)),
            std::runtime_error
        );

        objUpcast.set(std::move(valueDerived));
        REQUIRE(Count<Base>::moveAssigned() == 1);
        REQUIRE(Count<Derived>::moveAssigned() == 1);
        REQUIRE(objUpcast.get().getFrom() == &valueDerived);
    }

    SECTION("to an object referencing a mutable value.") {
        Base base;
        Derived derived;
        Reflect::Object<Base> objBase = std::ref(base);
        Reflect::Object<Base> objUpcast = std::ref(derived);
        Count<All>::clear();

        REQUIRE(objBase.getType() == Reflect::getType<Base &>());
        REQUIRE(objUpcast.getType() == Reflect::getType<Derived &>());

        objBase.set(std::move(valueBase));
        REQUIRE(Count<Base>::moveAssigned() == 1);
        REQUIRE(objBase.get().getFrom() == &valueBase);

        objBase.set(std::move(valueDerived));
        REQUIRE(Count<Base>::moveAssigned() == 1);
        REQUIRE(objBase.get().getFrom() == &valueDerived);

        REQUIRE_THROWS_AS(
            objUpcast.set(std::move(valueBase)),
            std::runtime_error
        );

        objUpcast.set(std::move(valueDerived));
        REQUIRE(Count<Base>::moveAssigned() == 1);
        REQUIRE(Count<Derived>::moveAssigned() == 1);
        REQUIRE(objUpcast.get().getFrom() == &valueDerived);
    }

    SECTION("to an object referencing a constant value.") {
        Base const base;
        Derived const derived;
        Reflect::Object<Base> objBase = std::ref(base);
        Reflect::Object<Base> objUpcast = std::ref(derived);
        Count<All>::clear();

        REQUIRE(objBase.getType() == Reflect::getType<Base const &>());
        REQUIRE(objUpcast.getType() == Reflect::getType<Derived const &>());

        REQUIRE_THROWS_AS(
            objBase.set(std::move(valueBase)),
            std::runtime_error
        );

        REQUIRE_THROWS_AS(
            objBase.set(std::move(valueDerived)),
            std::runtime_error
        );

        REQUIRE_THROWS_AS(
            objUpcast.set(std::move(valueBase)),
            std::runtime_error
        );

        REQUIRE_THROWS_AS(
            objUpcast.set(std::move(valueDerived)),
            std::runtime_error
        );
    }

    REQUIRE(Count<All>::clear());
}

TEST_CASE("Set object value by copying another object",
          "[object][access]") {
    SECTION("that owns its value.") {
        Reflect::Object<Base> objBase;
        Reflect::Object<Base> objUpcast = Derived();
        Reflect::Object<Base> target;
        Count<All>::clear();

        REQUIRE(objBase.getType() == Reflect::getType<Base>());
        REQUIRE(objUpcast.getType() == Reflect::getType<Derived>());
        REQUIRE(objBase.getType() == Reflect::getType<Base>());

        target.set(objBase);
        REQUIRE(Count<Base>::copyAssigned() == 1);
        REQUIRE(target.get().getFrom() == &objBase.get());

        target.set(objUpcast);
        REQUIRE(Count<Base>::copyAssigned() == 1);
        REQUIRE(target.get().getFrom() == &objUpcast.get());
    }

    SECTION("that references a mutable value.") {
        Base base;
        Derived derived;
        Reflect::Object<Base> objBase = std::ref(base);
        Reflect::Object<Base> objUpcast = std::ref(derived);
        Reflect::Object<Base> target;
        Count<All>::clear();

        REQUIRE(objBase.getType() == Reflect::getType<Base &>());
        REQUIRE(objUpcast.getType() == Reflect::getType<Derived &>());
        REQUIRE(target.getType() == Reflect::getType<Base>());

        target.set(objBase);
        REQUIRE(Count<Base>::copyAssigned() == 1);
        REQUIRE(target.get().getFrom() == &base);

        target.set(objUpcast);
        REQUIRE(Count<Base>::copyAssigned() == 1);
        REQUIRE(target.get().getFrom() == &derived);
    }

    SECTION("that references a constant value.") {
        Base const base;
        Derived const derived;
        Reflect::Object<Base> objBase = std::ref(base);
        Reflect::Object<Base> objUpcast = std::ref(derived);
        Reflect::Object<Base> target;
        Count<All>::clear();

        REQUIRE(objBase.getType() == Reflect::getType<Base const &>());
        REQUIRE(objUpcast.getType() == Reflect::getType<Derived const &>());
        REQUIRE(target.getType() == Reflect::getType<Base>());

        target.set(objBase);
        REQUIRE(Count<Base>::copyAssigned() == 1);
        REQUIRE(target.get().getFrom() == &base);

        target.set(objUpcast);
        REQUIRE(Count<Base>::copyAssigned() == 1);
        REQUIRE(target.get().getFrom() == &derived);
    }

    REQUIRE(Count<All>::clear());
}

TEST_CASE("Set object value by moving another object",
          "[object][access]") {
    SECTION("that owns its value.") {
        Reflect::Object<Base> objBase;
        Reflect::Object<Base> objUpcast = Derived();
        Reflect::Object<Base> target;
        Count<All>::clear();

        REQUIRE(objBase.getType() == Reflect::getType<Base>());
        REQUIRE(objUpcast.getType() == Reflect::getType<Derived>());
        REQUIRE(objBase.getType() == Reflect::getType<Base>());

        target.set(std::move(objBase));
        REQUIRE(Count<Base>::moveAssigned() == 1);
        REQUIRE(target.get().getFrom() == &objBase.get());

        target.set(std::move(objUpcast));
        REQUIRE(Count<Base>::moveAssigned() == 1);
        REQUIRE(target.get().getFrom() == &objUpcast.get());
    }

    SECTION("that references a mutable value.") {
        Base base;
        Derived derived;
        Reflect::Object<Base> objBase = std::ref(base);
        Reflect::Object<Base> objUpcast = std::ref(derived);
        Reflect::Object<Base> target;
        Count<All>::clear();

        REQUIRE(objBase.getType() == Reflect::getType<Base &>());
        REQUIRE(objUpcast.getType() == Reflect::getType<Derived &>());
        REQUIRE(target.getType() == Reflect::getType<Base>());

        target.set(std::move(objBase));
        REQUIRE(Count<Base>::moveAssigned() == 1);
        REQUIRE(target.get().getFrom() == &base);

        target.set(std::move(objUpcast));
        REQUIRE(Count<Base>::moveAssigned() == 1);
        REQUIRE(target.get().getFrom() == &derived);
    }

    SECTION("that references a constant value.") {
        Base const base;
        Derived const derived;
        Reflect::Object<Base> objBase = std::ref(base);
        Reflect::Object<Base> objUpcast = std::ref(derived);
        Reflect::Object<Base> target;
        Count<All>::clear();

        REQUIRE(objBase.getType() == Reflect::getType<Base const &>());
        REQUIRE(objUpcast.getType() == Reflect::getType<Derived const &>());
        REQUIRE(target.getType() == Reflect::getType<Base>());

        target.set(std::move(objBase));
        REQUIRE(Count<Base>::copyAssigned() == 1);
        REQUIRE(target.get().getFrom() == &base);

        target.set(std::move(objUpcast));
        REQUIRE(Count<Base>::copyAssigned() == 1);
        REQUIRE(target.get().getFrom() == &derived);
    }

    REQUIRE(Count<All>::clear());
}
