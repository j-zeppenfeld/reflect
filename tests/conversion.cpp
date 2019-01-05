// Copyright (c) 2019 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#include "common/catch.hpp"
#include "common/count.h"

#include "reflect/object.h"
#include "reflect/register.h"

//------------------------------------------------------------------------------
//--                            Class Convertible                             --
//------------------------------------------------------------------------------
namespace {
    class ConversionOperator;
    class ConversionMethod;
    class ConversionFunction;

    class Convertible : private Count<Convertible> {
    public:
        operator ConversionOperator() const;

        ConversionMethod convert() const;
    };

    class ConversionOperator : private Count<ConversionOperator> {
    public:
        ConversionOperator() = default;
        explicit ConversionOperator(Convertible const &from)
        : Count(from), _from(&from) { }

        Convertible const *getFrom() const {
            return _from;
        }

    private:
        Convertible const *_from = nullptr;
    };

    class ConversionMethod : private Count<ConversionMethod> {
    public:
        ConversionMethod() = default;
        explicit ConversionMethod(Convertible const &from)
        : Count(from), _from(&from) { }

        Convertible const *getFrom() const {
            return _from;
        }

    private:
        Convertible const *_from = nullptr;
    };

    class ConversionFunction : private Count<ConversionFunction> {
    public:
        ConversionFunction() = default;
        explicit ConversionFunction(Convertible const &from)
        : Count(from), _from(&from) { }

        Convertible const *getFrom() const {
            return _from;
        }

    private:
        Convertible const *_from = nullptr;
    };

    Convertible::operator ConversionOperator() const {
        return ConversionOperator(*this);
    }

    ConversionMethod Convertible::convert() const {
        return ConversionMethod(*this);
    }

    ConversionFunction convert(Convertible const &value) {
        return ConversionFunction(value);
    }

    struct ConvertibleRegistration {
        ConvertibleRegistration() {
            Reflect::Register<Convertible>()
                .conversion<ConversionOperator>()
                .conversion<ConversionMethod>(&Convertible::convert)
                .conversion<ConversionFunction>(&convert)
            ;
        }
    } convertibleRegistration;
}

//------------------------------------------------------------------------------
//--                                Test Cases                                --
//------------------------------------------------------------------------------

TEST_CASE("Get object value converted to another type.",
          "[object][access][convert]") {
    Convertible convertible;
    Reflect::Object<Convertible> obj = std::ref(convertible);
    Count<All>::clear();

    REQUIRE(obj.get<ConversionOperator>().getFrom() == &convertible);
    // Converted value was constructed directly into buffer,
    REQUIRE(Count<ConversionOperator>::valueConstructed() == 1);
    // then moved when returned from get.
    REQUIRE(Count<ConversionOperator>::moveConstructed() == 1);

    REQUIRE(obj.get<ConversionMethod>().getFrom() == &convertible);
    // Converted value was constructed in conversion method,
    REQUIRE(Count<ConversionMethod>::valueConstructed() == 1);
    // then moved into buffer and when returned from get.
    REQUIRE(Count<ConversionMethod>::moveConstructed() == 2);

    REQUIRE(obj.get<ConversionFunction>().getFrom() == &convertible);
    // Converted value was constructed in conversion function,
    REQUIRE(Count<ConversionFunction>::valueConstructed() == 1);
    // then moved into buffer and when returned from get.
    REQUIRE(Count<ConversionFunction>::moveConstructed() == 2);

    REQUIRE(Count<All>::clear());
}

TEST_CASE("Set object value by copy conversion",
          "[convert][object][access]") {
    Convertible convertible;
    Convertible objValue;
    Reflect::Object<Convertible> objConvertible = std::ref(objValue);

    SECTION("using conversion operator.") {
        Reflect::Object<ConversionOperator> obj;
        Count<All>::clear();

        REQUIRE_NOTHROW(obj.set(convertible));
        REQUIRE(obj.get().getFrom() == &convertible);
        REQUIRE(Count<ConversionOperator>::valueConstructed() == 1);
        REQUIRE(Count<ConversionOperator>::moveAssigned() == 1);

        REQUIRE_NOTHROW(obj.set(objConvertible));
        REQUIRE(obj.get().getFrom() == &objValue);
        REQUIRE(Count<ConversionOperator>::valueConstructed() == 1);
        REQUIRE(Count<ConversionOperator>::moveAssigned() == 1);
    }

    SECTION("using conversion method.") {
        Reflect::Object<ConversionMethod> obj;
        Count<All>::clear();

        REQUIRE_NOTHROW(obj.set(convertible));
        REQUIRE(obj.get().getFrom() == &convertible);
        REQUIRE(Count<ConversionMethod>::valueConstructed() == 1);
        REQUIRE(Count<ConversionMethod>::moveAssigned() == 1);

        REQUIRE_NOTHROW(obj.set(objConvertible));
        REQUIRE(obj.get().getFrom() == &objValue);
        REQUIRE(Count<ConversionMethod>::valueConstructed() == 1);
        REQUIRE(Count<ConversionMethod>::moveAssigned() == 1);
    }

    SECTION("using conversion function.") {
        Reflect::Object<ConversionFunction> obj;
        Count<All>::clear();

        REQUIRE_NOTHROW(obj.set(convertible));
        REQUIRE(obj.get().getFrom() == &convertible);
        REQUIRE(Count<ConversionFunction>::valueConstructed() == 1);
        REQUIRE(Count<ConversionFunction>::moveAssigned() == 1);

        REQUIRE_NOTHROW(obj.set(objConvertible));
        REQUIRE(obj.get().getFrom() == &objValue);
        REQUIRE(Count<ConversionFunction>::valueConstructed() == 1);
        REQUIRE(Count<ConversionFunction>::moveAssigned() == 1);
    }

    REQUIRE(Count<All>::clear());
}

TEST_CASE("Set object value by move conversion",
          "[convert][object][access]") {
    Convertible convertible;
    Convertible objValue;
    Reflect::Object<Convertible> objConvertible = std::ref(objValue);

    SECTION("using conversion operator.") {
        Reflect::Object<ConversionOperator> obj;
        Count<All>::clear();

        REQUIRE_NOTHROW(obj.set(std::move(convertible)));
        REQUIRE(obj.get().getFrom() == &convertible);
        REQUIRE(Count<ConversionOperator>::valueConstructed() == 1);
        REQUIRE(Count<ConversionOperator>::moveAssigned() == 1);

        REQUIRE_NOTHROW(obj.set(std::move(objConvertible)));
        REQUIRE(obj.get().getFrom() == &objValue);
        REQUIRE(Count<ConversionOperator>::valueConstructed() == 1);
        REQUIRE(Count<ConversionOperator>::moveAssigned() == 1);
    }

    SECTION("using conversion method.") {
        Reflect::Object<ConversionMethod> obj;
        Count<All>::clear();

        REQUIRE_NOTHROW(obj.set(std::move(convertible)));
        REQUIRE(obj.get().getFrom() == &convertible);
        REQUIRE(Count<ConversionMethod>::valueConstructed() == 1);
        REQUIRE(Count<ConversionMethod>::moveAssigned() == 1);

        REQUIRE_NOTHROW(obj.set(std::move(objConvertible)));
        REQUIRE(obj.get().getFrom() == &objValue);
        REQUIRE(Count<ConversionMethod>::valueConstructed() == 1);
        REQUIRE(Count<ConversionMethod>::moveAssigned() == 1);
    }

    SECTION("using conversion function.") {
        Reflect::Object<ConversionFunction> obj;
        Count<All>::clear();

        REQUIRE_NOTHROW(obj.set(std::move(convertible)));
        REQUIRE(obj.get().getFrom() == &convertible);
        REQUIRE(Count<ConversionFunction>::valueConstructed() == 1);
        REQUIRE(Count<ConversionFunction>::moveAssigned() == 1);

        REQUIRE_NOTHROW(obj.set(std::move(objConvertible)));
        REQUIRE(obj.get().getFrom() == &objValue);
        REQUIRE(Count<ConversionFunction>::valueConstructed() == 1);
        REQUIRE(Count<ConversionFunction>::moveAssigned() == 1);
    }

    REQUIRE(Count<All>::clear());
}
