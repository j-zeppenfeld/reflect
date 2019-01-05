// Copyright (c) 2019 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#include "common/catch.hpp"
#include "common/classes.h"

#include "reflect/object.h"
#include "reflect/register.h"

//------------------------------------------------------------------------------
//--                               Registration                               --
//------------------------------------------------------------------------------

namespace {
    struct Registration {
        Registration() {
            Reflect::Register<Derived>()
                .base<Base>()
            ;
        }
    } registration;
}

//------------------------------------------------------------------------------
//--                                Test Cases                                --
//------------------------------------------------------------------------------

TEST_CASE("Construct object by constructing its value in place",
          "[object][construct][by-value]") {
    Count<All>::clear();

    SECTION("using the default-constructor.") {
        Reflect::Object<Base> obj;
        REQUIRE(Count<Base>::defaultConstructed() == 1);
        REQUIRE(obj.getType() == Reflect::getType<Base>());
    }

    SECTION("from a scalar value.") {
        Reflect::Object<Base> obj = 42;
        REQUIRE(Count<Base>::valueConstructed() == 1);
        REQUIRE(obj.getType() == Reflect::getType<Base>());
        REQUIRE(obj.get().getInt() == 42);
    }

    SECTION("from a nonscalar value.") {
        Reflect::Object<Base> obj = std::string("hello");
        REQUIRE(Count<Base>::valueConstructed() == 1);
        REQUIRE(obj.getType() == Reflect::getType<Base>());
        REQUIRE(obj.get().getString() == "hello");
    }

    SECTION("from multiple values.") {
        Reflect::Object<Base> obj {314, std::string("world")};
        REQUIRE(Count<Base>::valueConstructed() == 1);
        REQUIRE(obj.getType() == Reflect::getType<Base>());
        REQUIRE(obj.get().getInt() == 314);
        REQUIRE(obj.get().getString() == "world");
    }

    SECTION("from an incompatible type.") {
        REQUIRE(!std::is_constructible<
            Reflect::Object<Base>, Base *
        >::value);

        REQUIRE(!std::is_constructible<
            Reflect::Object<Base>, Unrelated
        >::value);
    }

    REQUIRE(Count<All>::clear());
}

TEST_CASE("Construct object by copying a value",
          "[object][construct][by-value]") {
    Base base;
    Derived derived;
    Count<All>::clear();

    SECTION("of the same type.") {
        Reflect::Object<Base> obj = base;
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(obj.get().getFrom() == &base);
        REQUIRE(obj.getType() == Reflect::getType<Base>());
    }

    SECTION("of derived type.") {
        Reflect::Object<Base> obj = derived;
        REQUIRE(Count<Derived>::copyConstructed() == 1);
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(obj.get().getFrom() == &derived);
        REQUIRE(obj.getType() == Reflect::getType<Derived>());
    }

    REQUIRE(Count<All>::clear());
}

TEST_CASE("Construct object by moving a value",
          "[object][construct][by-value]") {
    Base base;
    Derived derived;
    Count<All>::clear();

    SECTION("of the same type.") {
        Reflect::Object<Base> obj = std::move(base);
        REQUIRE(Count<Base>::moveConstructed() == 1);
        REQUIRE(obj.get().getFrom() == &base);
        REQUIRE(obj.getType() == Reflect::getType<Base>());
    }

    SECTION("of derived type.") {
        Reflect::Object<Base> obj = std::move(derived);
        REQUIRE(Count<Derived>::moveConstructed() == 1);
        REQUIRE(Count<Base>::moveConstructed() == 1);
        REQUIRE(obj.get().getFrom() == &derived);
        REQUIRE(obj.getType() == Reflect::getType<Derived>());
    }

    REQUIRE(Count<All>::clear());
}

TEST_CASE("Construct object by copying another object",
          "[object][construct][by-value]") {
    SECTION("of the same type.") {
        Base base;
        Reflect::Object<Base> objBase = base;
        Reflect::Object<Base> refBase = std::ref(base);
        Reflect::Object<Base> crefBase = std::cref(base);
        Count<All>::clear();

        Reflect::Object<Base> obj = objBase;
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(obj.get().getFrom() == &objBase.get());
        REQUIRE(obj.getType() == Reflect::getType<Base>());

        Reflect::Object<Base> ref = refBase;
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(ref.get().getFrom() == &base);
        REQUIRE(ref.getType() == Reflect::getType<Base>());

        Reflect::Object<Base> cref = crefBase;
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(cref.get().getFrom() == &base);
        REQUIRE(cref.getType() == Reflect::getType<Base>());
    }

    SECTION("of upcast type.") {
        Derived derived;
        Reflect::Object<Base> objUpcast = derived;
        Reflect::Object<Base> refUpcast = std::ref(derived);
        Reflect::Object<Base> crefUpcast = std::cref(derived);
        Count<All>::clear();

        Reflect::Object<Base> obj = objUpcast;
        REQUIRE(Count<Derived>::copyConstructed() == 1);
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(obj.get().getFrom() == &objUpcast.get());
        REQUIRE(obj.getType() == Reflect::getType<Derived>());

        Reflect::Object<Base> ref = refUpcast;
        REQUIRE(Count<Derived>::copyConstructed() == 1);
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(ref.get().getFrom() == &derived);
        REQUIRE(ref.getType() == Reflect::getType<Derived>());

        Reflect::Object<Base> cref = crefUpcast;
        REQUIRE(Count<Derived>::copyConstructed() == 1);
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(cref.get().getFrom() == &derived);
        REQUIRE(cref.getType() == Reflect::getType<Derived>());
    }

    SECTION("of derived type.") {
        Derived derived;
        Reflect::Object<Derived> objDerived = derived;
        Reflect::Object<Derived> refDerived = std::ref(derived);
        Reflect::Object<Derived> crefDerived = std::cref(derived);
        Count<All>::clear();

        Reflect::Object<Base> obj = objDerived;
        REQUIRE(Count<Derived>::copyConstructed() == 1);
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(obj.get().getFrom() == &objDerived.get());
        REQUIRE(obj.getType() == Reflect::getType<Derived>());

        Reflect::Object<Base> ref = refDerived;
        REQUIRE(Count<Derived>::copyConstructed() == 1);
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(ref.get().getFrom() == &derived);
        REQUIRE(ref.getType() == Reflect::getType<Derived>());

        Reflect::Object<Base> cref = crefDerived;
        REQUIRE(Count<Derived>::copyConstructed() == 1);
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(cref.get().getFrom() == &derived);
        REQUIRE(cref.getType() == Reflect::getType<Derived>());
    }

    REQUIRE(Count<All>::clear());
}

TEST_CASE("Construct object by moving another object",
          "[object][construct][by-value]") {
    SECTION("of the same type.") {
        Base base;
        Reflect::Object<Base> objBase = base;
        Reflect::Object<Base> refBase = std::ref(base);
        Reflect::Object<Base> crefBase = std::cref(base);
        Count<All>::clear();

        Reflect::Object<Base> obj = std::move(objBase);
        REQUIRE(Count<Base>::moveConstructed() == 1);
        REQUIRE(obj.get().getFrom() == &objBase.get());
        REQUIRE(obj.getType() == Reflect::getType<Base>());

        Reflect::Object<Base> ref = std::move(refBase);
        REQUIRE(Count<Base>::moveConstructed() == 1);
        REQUIRE(ref.get().getFrom() == &base);
        REQUIRE(ref.getType() == Reflect::getType<Base>());

        Reflect::Object<Base> cref = std::move(crefBase);
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(cref.get().getFrom() == &base);
        REQUIRE(cref.getType() == Reflect::getType<Base>());
    }

    SECTION("of upcast type.") {
        Derived derived;
        Reflect::Object<Base> objUpcast = derived;
        Reflect::Object<Base> refUpcast = std::ref(derived);
        Reflect::Object<Base> crefUpcast = std::cref(derived);
        Count<All>::clear();

        Reflect::Object<Base> obj = std::move(objUpcast);
        REQUIRE(Count<Derived>::moveConstructed() == 1);
        REQUIRE(Count<Base>::moveConstructed() == 1);
        REQUIRE(obj.get().getFrom() == &objUpcast.get());
        REQUIRE(obj.getType() == Reflect::getType<Derived>());

        Reflect::Object<Base> ref = std::move(refUpcast);
        REQUIRE(Count<Derived>::moveConstructed() == 1);
        REQUIRE(Count<Base>::moveConstructed() == 1);
        REQUIRE(ref.get().getFrom() == &derived);
        REQUIRE(ref.getType() == Reflect::getType<Derived>());

        Reflect::Object<Base> cref = std::move(crefUpcast);
        REQUIRE(Count<Derived>::copyConstructed() == 1);
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(cref.get().getFrom() == &derived);
        REQUIRE(cref.getType() == Reflect::getType<Derived>());
    }

    SECTION("of derived type.") {
        Derived derived;
        Reflect::Object<Derived> objDerived = derived;
        Reflect::Object<Derived> refDerived = std::ref(derived);
        Reflect::Object<Derived> crefDerived = std::cref(derived);
        Count<All>::clear();

        Reflect::Object<Base> obj = std::move(objDerived);
        REQUIRE(Count<Derived>::moveConstructed() == 1);
        REQUIRE(Count<Base>::moveConstructed() == 1);
        REQUIRE(obj.get().getFrom() == &objDerived.get());
        REQUIRE(obj.getType() == Reflect::getType<Derived>());

        Reflect::Object<Base> ref = std::move(refDerived);
        REQUIRE(Count<Derived>::moveConstructed() == 1);
        REQUIRE(Count<Base>::moveConstructed() == 1);
        REQUIRE(ref.get().getFrom() == &derived);
        REQUIRE(ref.getType() == Reflect::getType<Derived>());

        Reflect::Object<Base> cref = std::move(crefDerived);
        REQUIRE(Count<Derived>::copyConstructed() == 1);
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(cref.get().getFrom() == &derived);
        REQUIRE(cref.getType() == Reflect::getType<Derived>());
    }

    REQUIRE(Count<All>::clear());
}

TEST_CASE("Construct object by referencing a value",
          "[object][construct][by-reference]") {
    Base base;
    Derived derived;
    Count<All>::clear();

    SECTION("of the same type.") {
        Reflect::Object<Base> obj = std::ref(base);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&obj.get() == &base);
        REQUIRE(obj.getType() == Reflect::getType<Base &>());
    }

    SECTION("of derived type.") {
        Reflect::Object<Base> obj = std::ref(derived);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&obj.get() == &derived);
        REQUIRE(obj.getType() == Reflect::getType<Derived &>());
    }

    SECTION("of the same constant type.") {
        Reflect::Object<Base> obj = std::cref(base);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&obj.get<Base const &>() == &base);
        REQUIRE(obj.getType() == Reflect::getType<Base const &>());
    }

    SECTION("of derived constant type.") {
        Reflect::Object<Base> obj = std::cref(derived);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&obj.get<Base const &>() == &derived);
        REQUIRE(obj.getType() == Reflect::getType<Derived const &>());
    }

    REQUIRE(Count<All>::clear());
}

TEST_CASE("Construct object by referencing another object",
          "[object][construct][by-reference]") {
    SECTION("of the same type.") {
        Base base;
        Reflect::Object<Base> objBase = base;
        Reflect::Object<Base> refBase = std::ref(base);
        Reflect::Object<Base> crefBase = std::cref(base);
        Count<All>::clear();

        Reflect::Object<Base> obj = std::ref(objBase);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&obj.get() == &objBase.get());
        REQUIRE(obj.getType() == Reflect::getType<Base &>());

        Reflect::Object<Base> ref = std::ref(refBase);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&ref.get() == &base);
        REQUIRE(ref.getType() == Reflect::getType<Base &>());

        Reflect::Object<Base> cref = std::ref(crefBase);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&cref.get<Base const &>() == &base);
        REQUIRE(cref.getType() == Reflect::getType<Base const &>());
    }

    SECTION("of upcast type.") {
        Derived derived;
        Reflect::Object<Base> objUpcast = derived;
        Reflect::Object<Base> refUpcast = std::ref(derived);
        Reflect::Object<Base> crefUpcast = std::cref(derived);
        Count<All>::clear();

        Reflect::Object<Base> obj = std::ref(objUpcast);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&obj.get() == &objUpcast.get());
        REQUIRE(obj.getType() == Reflect::getType<Derived &>());

        Reflect::Object<Base> ref = std::ref(refUpcast);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&ref.get() == &derived);
        REQUIRE(ref.getType() == Reflect::getType<Derived &>());

        Reflect::Object<Base> cref = std::ref(crefUpcast);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&cref.get<Base const &>() == &derived);
        REQUIRE(cref.getType() == Reflect::getType<Derived const &>());
    }

    SECTION("of derived type.") {
        Derived derived;
        Reflect::Object<Derived> objDerived = derived;
        Reflect::Object<Derived> refDerived = std::ref(derived);
        Reflect::Object<Derived> crefDerived = std::cref(derived);
        Count<All>::clear();

        Reflect::Object<Base> obj = std::ref(objDerived);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&obj.get() == &objDerived.get());
        REQUIRE(obj.getType() == Reflect::getType<Derived &>());

        Reflect::Object<Base> ref = std::ref(refDerived);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&ref.get() == &derived);
        REQUIRE(ref.getType() == Reflect::getType<Derived &>());

        Reflect::Object<Base> cref = std::ref(crefDerived);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&cref.get<Base const &>() == &derived);
        REQUIRE(cref.getType() == Reflect::getType<Derived const &>());
    }

    SECTION("of the same constant type.") {
        Base base;
        Reflect::Object<Base> const objBase = base;
        Reflect::Object<Base> const refBase = std::ref(base);
        Reflect::Object<Base> const crefBase = std::cref(base);
        Count<All>::clear();

        Reflect::Object<Base> obj = std::ref(objBase);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&obj.get<Base const &>() == &objBase.get());
        REQUIRE(obj.getType() == Reflect::getType<Base const &>());

        Reflect::Object<Base> ref = std::ref(refBase);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&ref.get<Base const &>() == &base);
        REQUIRE(ref.getType() == Reflect::getType<Base const &>());

        Reflect::Object<Base> cref = std::ref(crefBase);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&cref.get<Base const &>() == &base);
        REQUIRE(cref.getType() == Reflect::getType<Base const &>());
    }

    SECTION("of upcast constant type.") {
        Derived derived;
        Reflect::Object<Base> const objUpcast = derived;
        Reflect::Object<Base> const refUpcast = std::ref(derived);
        Reflect::Object<Base> const crefUpcast = std::cref(derived);
        Count<All>::clear();

        Reflect::Object<Base> obj = std::ref(objUpcast);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&obj.get<Base const &>() == &objUpcast.get());
        REQUIRE(obj.getType() == Reflect::getType<Derived const &>());

        Reflect::Object<Base> ref = std::ref(refUpcast);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&ref.get<Base const &>() == &derived);
        REQUIRE(ref.getType() == Reflect::getType<Derived const &>());

        Reflect::Object<Base> cref = std::ref(crefUpcast);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&cref.get<Base const &>() == &derived);
        REQUIRE(cref.getType() == Reflect::getType<Derived const &>());
    }

    SECTION("of derived constant type.") {
        Derived derived;
        Reflect::Object<Derived> const objDerived = derived;
        Reflect::Object<Derived> const refDerived = std::ref(derived);
        Reflect::Object<Derived> const crefDerived = std::cref(derived);
        Count<All>::clear();

        Reflect::Object<Base> obj = std::ref(objDerived);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&obj.get<Base const &>() == &objDerived.get());
        REQUIRE(obj.getType() == Reflect::getType<Derived const &>());

        Reflect::Object<Base> ref = std::ref(refDerived);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&ref.get<Base const &>() == &derived);
        REQUIRE(ref.getType() == Reflect::getType<Derived const &>());

        Reflect::Object<Base> cref = std::ref(crefDerived);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&cref.get<Base const &>() == &derived);
        REQUIRE(cref.getType() == Reflect::getType<Derived const &>());
    }

    REQUIRE(Count<All>::clear());
}

TEST_CASE("Construct object of void type",
          "[object][construct][by-value]") {
    SECTION("using the default-constructor.") {
        Reflect::Object<> obj;
        REQUIRE(obj.getType() == Reflect::getType<void>());
    }

    SECTION("by copying a scalar value.") {
        Reflect::Object<> obj = 27;
        REQUIRE(obj.getType() == Reflect::getType<int>());
        REQUIRE(obj.get<int>() == 27);
    }

    SECTION("by copying a nonscalar value.") {
        Base base;
        Count<All>::clear();

        Reflect::Object<> obj = base;
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(obj.get<Base &>().getFrom() == &base);
        REQUIRE(obj.getType() == Reflect::getType<Base>());
    }

    SECTION("by moving a nonscalar value.") {
        Base base;
        Count<All>::clear();

        Reflect::Object<> obj = std::move(base);
        REQUIRE(Count<Base>::moveConstructed() == 1);
        REQUIRE(obj.get<Base &>().getFrom() == &base);
        REQUIRE(obj.getType() == Reflect::getType<Base>());
    }

    SECTION("by copying another object of void type.") {
        Reflect::Object<> objVoid;

        Reflect::Object<> obj = objVoid;
        REQUIRE(obj.getType() == Reflect::getType<void>());
    }

    SECTION("by copying another object of any type.") {
        Reflect::Object<Base> objBase;
        Count<All>::clear();

        Reflect::Object<> obj = objBase;
        REQUIRE(Count<Base>::copyConstructed() == 1);
        REQUIRE(obj.get<Base &>().getFrom() == &objBase.get<Base &>());
        REQUIRE(obj.getType() == Reflect::getType<Base>());
    }

    SECTION("by moving another object of void type.") {
        Reflect::Object<> objVoid;

        Reflect::Object<> obj = std::move(objVoid);
        REQUIRE(obj.getType() == Reflect::getType<void>());
    }

    SECTION("by moving another object of any type.") {
        Reflect::Object<Base> objBase;
        Count<All>::clear();

        Reflect::Object<> obj = std::move(objBase);
        REQUIRE(Count<Base>::moveConstructed() == 1);
        REQUIRE(obj.get<Base &>().getFrom() == &objBase.get<Base &>());
        REQUIRE(obj.getType() == Reflect::getType<Base>());
    }

    SECTION("by referencing a scalar value.") {
        int i = 2718;

        Reflect::Object<> ref = std::ref(i);
        REQUIRE(&ref.get<int &>() == &i);
        REQUIRE(ref.getType() == Reflect::getType<int &>());

        Reflect::Object<> cref = std::cref(i);
        REQUIRE(&cref.get<int const &>() == &i);
        REQUIRE(cref.getType() == Reflect::getType<int const &>());
    }

    SECTION("by referencing a nonscalar value.") {
        Base base;
        Count<All>::clear();

        Reflect::Object<> ref = std::ref(base);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&ref.get<Base &>() == &base);
        REQUIRE(ref.getType() == Reflect::getType<Base &>());

        Reflect::Object<> cref = std::cref(base);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&cref.get<Base const &>() == &base);
        REQUIRE(cref.getType() == Reflect::getType<Base const &>());
    }

    SECTION("by referencing another object of void type.") {
        Reflect::Object<> objVoid;

        Reflect::Object<> ref = std::ref(objVoid);
        REQUIRE(ref.getType() == Reflect::getType<void>());

        Reflect::Object<> cref = std::cref(objVoid);
        REQUIRE(cref.getType() == Reflect::getType<void>());
    }

    SECTION("by referencing another object of any type.") {
        Reflect::Object<Base> objBase;
        Count<All>::clear();

        Reflect::Object<> ref = std::ref(objBase);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&ref.get<Base &>() == &objBase.get<Base &>());
        REQUIRE(ref.getType() == Reflect::getType<Base &>());

        Reflect::Object<> cref = std::cref(objBase);
        REQUIRE(Count<All>::constructed() == 0);
        REQUIRE(&cref.get<Base const &>() == &objBase.get<Base &>());
        REQUIRE(cref.getType() == Reflect::getType<Base const &>());
    }

    REQUIRE(Count<All>::clear());
}
