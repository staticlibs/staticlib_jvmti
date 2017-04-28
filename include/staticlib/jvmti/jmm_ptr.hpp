/*
 * Copyright 2017, alex at staticlibs.net
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE.md file that
 * accompanied this code).
 */

/* 
 * File:   jmm_ptr.hpp
 * Author: alex
 *
 * Created on April 23, 2017, 4:20 PM
 */

#ifndef STATICLIB_JVMTI_JMM_PTR_HPP
#define	STATICLIB_JVMTI_JMM_PTR_HPP

#include "jni.h"
#include "jvmti.h"
#include "jvm.h"
#include "jmm.h"

#include "staticlib/config.hpp"
#include "staticlib/jni.hpp"
#include "staticlib/support.hpp"

#include "staticlib/jvmti/jvmti_exception.hpp"

namespace staticlib {
namespace jvmti {

/**
 * Wraps `JmmInterface` pointer and provides helpers to call JMM methods
 */
class jmm_ptr {
    // no destruction is required
    sl::support::observer_ptr<JmmInterface> jmm;

public:
    /**
     * Constructor, creates empty object
     * 
     * @param nothing null pointer
     */
    jmm_ptr(std::nullptr_t nothing) :
    jmm(nothing) { }
    
    /**
     * Constructor, obtains `JmmInterface` pointer from global JVM pointer
     * 
     * @throws jvmti_exception
     */
    jmm_ptr() :
    jmm([] {
        auto scoped = sl::jni::thread_local_jni_env_ptr();
        auto res = JVM_GetManagement(JMM_VERSION_1_0);
        if (nullptr == res) {
            throw jvmti_exception(TRACEMSG("Error obtaining JMM interface"));
        }
        return sl::support::observer_ptr<JmmInterface>(static_cast<JmmInterface*>(res));
    }()) { }

    /**
     * Copy constructor
     * 
     * @param other other instance
     */
    jmm_ptr(const jmm_ptr& other) :
    jmm(other.jmm) { }

    /**
     * Copy assignment operator
     * 
     * @param other other instance
     * @return this instance
     */
    jmm_ptr& operator=(const jmm_ptr& other) {
        jmm = other.jmm;
        return *this;
    }

    /**
     * Provides access to `JmmInterface` pointer
     * 
     * @returns pointer to `JmmInterface`
     */
    JmmInterface* operator->() {
        return jmm.get();
    }

    /**
     * Provides access to `JmmInterface` pointer
     * 
     * @returns pointer to `JmmInterface`
     */
    JmmInterface* get() {
        return jmm.get();
    }

    /**
     * Calls arbitrary JMM method
     * 
     * @param func pointer to `JmmInterface's`member method to use for calling
     * @param args method arguments
     * @return jvmti_exception on java exception
     */
    template<typename Result, typename Func, typename... Args>
    Result call_method(Func func, Args... args) {
        auto env = sl::jni::thread_local_jni_env_ptr();
        Result res = (jmm.get()->*func)(env.get(), args...);
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
            throw jvmti_exception(TRACEMSG("Exception raised when calling JMM method"));
        }
        return res;
    }

    /**
     * Calls arbitrary JMM method, that returns java object
     * 
     * @param resclass resulting class
     * @param func pointer to `JmmInterface's`member method to use for calling
     * @param args method arguments
     * @return jvmti_exception on java exception
     */
    template<typename Func, typename... Args>
    sl::jni::jobject_ptr call_object_method(const sl::jni::jclass_ptr& resclass, Func func, Args... args) {
        auto scoped = sl::jni::thread_local_jni_env_ptr();
        jobject local = call_method<jobject>(func, args...);
        if (nullptr == local) {
            throw jvmti_exception(TRACEMSG("Invalid null object returned by JMM method"));
        }
        return sl::jni::jobject_ptr(resclass, local);
    }
};

} // namespace
}

#endif	/* STATICLIB_JVMTI_JMM_PTR_HPP */

