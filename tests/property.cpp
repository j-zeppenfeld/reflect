// Copyright (c) 2019 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#include "common/catch.hpp"
#include "common/classes.h"

#include "reflect/object.h"
#include "reflect/register.h"

//------------------------------------------------------------------------------
//--                             Class Propertied                             --
//------------------------------------------------------------------------------
namespace {
	class Propertied {
	public:
		Propertied(int baseValue, int derivedValue)
		: _base(baseValue)
		, _derived(derivedValue)
		, _cBase(baseValue)
		, _cDerived(derivedValue)
		, _mBase(baseValue)
		, _mDerived(derivedValue) { }

	public:
		// member
		Base _base;
		Derived _derived;

		// member const
		Base const _cBase;
		Derived const _cDerived;
		
		// mutable member
		Base mutable _mBase;
		Derived mutable _mDerived;
		
		// value get()
		Base getBase() { return _base; }
		Derived getDerived() { return _derived; }
		
		// value get() const
		Base getBaseC() const { return _base; }
		Derived getDerivedC() const { return _derived; }
		
		// value &get()
		Base &getRBase() { return _base; }
		Derived &getRDerived() { return _derived; }
		
		// value &get() const
		Base &getRBaseC() const { return _mBase; }
		Derived &getRDerivedC() const { return _mDerived; }
		
		// value const &get()
		Base const &getRCBase() { return _base; }
		Derived const &getRCDerived() { return _derived; }
		
		// value const &get() const
		Base const &getRCBaseC() const { return _base; }
		Derived const &getRCDerivedC() const { return _derived; }

		// void set(value)
		void setBase(Base value) { _base = value; }
		void setDerived(Derived value) { _derived = value; }
		
		// void set(value &)
		void setRBase(Base &value) { _base = value; }
		void setRDerived(Derived &value) { _derived = value; }
		
		// void set(value const &)
		void setRCBase(Base const &value) { _base = value; }
		void setRCDerived(Derived const &value) { _base = value; }


		Propertied(int value) : _member(value) { }

		Propertied &operator=(Propertied const &other) {
			_member = other._member;
			return *this;
		}

		int _member;
		const int _cmember = 11;

		int getValue() const { return _member; }
		void setValue(int value) { _member = value; }

		int &getReference() { return _member; }
		int const &getReference() const { return _member; }
		int &getMutableReference() { return _member; }
		int const &getConstReference() const { return _member; }
		void setReference(int const &value) { _member = value; }

		Propertied nested() const { return (2 * _member); }
	};

	struct PropertiedRegistration {
		PropertiedRegistration() {
			Reflect::Register<Propertied>("Propertied")
				.property("member", &Propertied::_member)
				.property("cmember", &Propertied::_cmember)
				.property("value", &Propertied::getValue, &Propertied::setValue)
				.property("value_get", &Propertied::getValue)
				.property("value_set", &Propertied::setValue)
				.property("ref", &Propertied::getReference,
				                 &Propertied::getReference,
				                 &Propertied::setReference)
				.property("ref_mutable", &Propertied::getMutableReference)
				.property("ref_constant", &Propertied::getConstReference)
				.property("nested", &Propertied::nested)
			;
		}
	} propertiedRegistration;
}

//------------------------------------------------------------------------------
//--                                Test Cases                                --
//------------------------------------------------------------------------------

TEST_CASE("Temporary") {
	SECTION("Mutable object.") {
		Reflect::Object<Propertied> obj = 6749735;
		REQUIRE(obj.getProperty("member").get<int>() == 6749735);
		REQUIRE(obj.getProperty("cmember").get<int>() == 11);
		REQUIRE(obj.getProperty("value_get").get<int>() == 6749735);
		REQUIRE(obj.getProperty("ref").get<int>() == 6749735);
		
		obj.getProperty("value_set").set(42);
		REQUIRE(obj.getProperty("value_get").get<int>() == 42);
		
		obj.getProperty("member").set(11);
		REQUIRE(obj.getProperty("ref").get<int>() == 11);

		REQUIRE(obj.getProperty("nested").getProperty("value").get<int>() == 22);
	}
	
	SECTION("Constant object.") {
		Reflect::Object<Propertied> const obj = 1234;
		REQUIRE(obj.getProperty("member").get<int>() == 1234);
		REQUIRE(obj.getProperty("cmember").get<int>() == 11);
		REQUIRE(obj.getProperty("value_get").get<int>() == 1234);
		REQUIRE(obj.getProperty("ref").get<int>() == 1234);
	}
}
