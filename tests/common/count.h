// Copyright (c) 2018 Johannes Zeppenfeld
// SPDX-License-Identifier: MIT

#ifndef REFLECT_TESTS_COMMON_COUNT_H
#define REFLECT_TESTS_COMMON_COUNT_H

#include <vector>

//------------------------------------------------------------------------------
//--                               Class Count                                --
//------------------------------------------------------------------------------
template <typename T> class Count;

struct All { };

template<>
class Count<All> {
public:
    static uint32_t constructed() {
        uint32_t count = 0;
        for(auto counter : _counters()) {
            count += counter->constructed();
        }
        return count;
    }

    static uint32_t defaultConstructed() {
        uint32_t count = 0;
        for(auto counter : _counters()) {
            count += counter->defaultConstructed();
        }
        return count;
    }

    static uint32_t valueConstructed() {
        uint32_t count = 0;
        for(auto counter : _counters()) {
            count += counter->valueConstructed();
        }
        return count;
    }

    static uint32_t copyConstructed() {
        uint32_t count = 0;
        for(auto counter : _counters()) {
            count += counter->copyConstructed();
        }
        return count;
    }

    static uint32_t moveConstructed() {
        uint32_t count = 0;
        for(auto counter : _counters()) {
            count += counter->moveConstructed();
        }
        return count;
    }

    static uint32_t assigned() {
        uint32_t count = 0;
        for(auto counter : _counters()) {
            count += counter->assigned();
        }
        return count;
    }

    static uint32_t valueAssigned() {
        uint32_t count = 0;
        for(auto counter : _counters()) {
            count += counter->valueAssigned();
        }
        return count;
    }

    static uint32_t copyAssigned() {
        uint32_t count = 0;
        for(auto counter : _counters()) {
            count += counter->copyAssigned();
        }
        return count;
    }

    static uint32_t moveAssigned() {
        uint32_t count = 0;
        for(auto counter : _counters()) {
            count += counter->moveAssigned();
        }
        return count;
    }

    static bool clear() {
        bool all = true;
        for(auto counter : _counters()) {
            all &= counter->clear();
        }
        return all;
    }

protected:
    template <typename T>
    Count(Count<T> &) {
        static struct : public Counter {
            uint32_t constructed() const override {
                return Count<T>::constructed();
            }
            uint32_t defaultConstructed() const override {
                return Count<T>::defaultConstructed();
            }
            uint32_t valueConstructed() const override {
                return Count<T>::valueConstructed();
            }
            uint32_t copyConstructed() const override {
                return Count<T>::copyConstructed();
            }
            uint32_t moveConstructed() const override {
                return Count<T>::moveConstructed();
            }
            uint32_t assigned() const override {
                return Count<T>::moveConstructed();
            }
            uint32_t valueAssigned() const override {
                return Count<T>::valueAssigned();
            }
            uint32_t copyAssigned() const override {
                return Count<T>::copyAssigned();
            }
            uint32_t moveAssigned() const override {
                return Count<T>::moveAssigned();
            }
            bool clear() const override {
                return Count<T>::clear();
            }
        } counter;
    }

private:
    class Counter {
    public:
        Counter() { _counters().push_back(this); }
        virtual uint32_t constructed() const = 0;
        virtual uint32_t defaultConstructed() const = 0;
        virtual uint32_t valueConstructed() const = 0;
        virtual uint32_t copyConstructed() const = 0;
        virtual uint32_t moveConstructed() const = 0;
        virtual uint32_t assigned() const = 0;
        virtual uint32_t valueAssigned() const = 0;
        virtual uint32_t copyAssigned() const = 0;
        virtual uint32_t moveAssigned() const = 0;
        virtual bool clear() const = 0;
    };

    static std::vector<Counter const *> &_counters() {
        static std::vector<Counter const *> counters;
        return counters;
    }
};

template <typename T>
class Count : private Count<All> {
public:
    static uint32_t constructed() {
        uint32_t count = _defaultConstructed
                       + _valueConstructed
                       + _copyConstructed
                       + _moveConstructed;
        _defaultConstructed = 0;
        _valueConstructed = 0;
        _copyConstructed = 0;
        _moveConstructed = 0;
        return count;
    }

    static uint32_t defaultConstructed() {
        uint32_t count = _defaultConstructed;
        _defaultConstructed = 0;
        return count;
    }

    static uint32_t valueConstructed() {
        uint32_t count = _valueConstructed;
        _valueConstructed = 0;
        return count;
    }

    static uint32_t copyConstructed() {
        uint32_t count = _copyConstructed;
        _copyConstructed = 0;
        return count;
    }

    static uint32_t moveConstructed() {
        uint32_t count = _moveConstructed;
        _moveConstructed = 0;
        return count;
    }

    static uint32_t assigned() {
        uint32_t count = _valueAssigned
                       + _copyAssigned
                       + _moveAssigned;
        _valueAssigned = 0;
        _copyAssigned = 0;
        _moveAssigned = 0;
        return count;
    }

    static uint32_t valueAssigned() {
        uint32_t count = _valueAssigned;
        _valueAssigned = 0;
        return count;
    }

    static uint32_t copyAssigned() {
        uint32_t count = _copyAssigned;
        _copyAssigned = 0;
        return count;
    }

    static uint32_t moveAssigned() {
        uint32_t count = _moveAssigned;
        _moveAssigned = 0;
        return count;
    }

    static bool clear() {
        uint32_t count = _defaultConstructed
                       + _valueConstructed
                       + _copyConstructed
                       + _moveConstructed
                       + _valueAssigned
                       + _copyAssigned
                       + _moveAssigned;
        _defaultConstructed = 0;
        _valueConstructed = 0;
        _copyConstructed = 0;
        _moveConstructed = 0;
        _valueAssigned = 0;
        _copyAssigned = 0;
        _moveAssigned = 0;
        return !count;
    }

protected:
    Count() : Count<All>(*this) {
        ++_defaultConstructed;
    }

    template <
        typename T_Arg, typename ...T_Args,
        typename = typename std::enable_if<
            sizeof...(T_Args) != 0 ||
            !std::is_base_of<Count, typename std::decay<T_Arg>::type>::value
        >::type
    >
    Count(T_Arg &&, T_Args &&...) : Count<All>(*this) {
        ++_valueConstructed;
    }

    Count(Count const &) : Count<All>(*this) {
        ++_copyConstructed;
    }

    Count(Count &&) : Count<All>(*this) {
        ++_moveConstructed;
    }

    template <
        typename T_Arg,
        typename = typename std::enable_if<
            !std::is_base_of<Count, typename std::decay<T_Arg>::type>::value
        >::type
    >
    Count &operator=(T_Arg &&) {
        ++_valueAssigned;
        return *this;
    }

    Count &operator=(Count const &) {
        ++_copyAssigned;
        return *this;
    }

    Count &operator=(Count &&) {
        ++_moveAssigned;
        return *this;
    }

private:
    static uint32_t _defaultConstructed;
    static uint32_t _valueConstructed;
    static uint32_t _copyConstructed;
    static uint32_t _moveConstructed;
    static uint32_t _valueAssigned;
    static uint32_t _copyAssigned;
    static uint32_t _moveAssigned;
};

template <typename T> uint32_t Count<T>::_defaultConstructed = 0;
template <typename T> uint32_t Count<T>::_valueConstructed = 0;
template <typename T> uint32_t Count<T>::_copyConstructed = 0;
template <typename T> uint32_t Count<T>::_moveConstructed = 0;
template <typename T> uint32_t Count<T>::_valueAssigned = 0;
template <typename T> uint32_t Count<T>::_copyAssigned = 0;
template <typename T> uint32_t Count<T>::_moveAssigned = 0;

#endif
