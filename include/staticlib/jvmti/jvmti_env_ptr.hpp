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
 * File:   jvmti_env_ptr.hpp
 * Author: alex
 *
 * Created on April 22, 2017, 8:39 PM
 */

#ifndef STATICLIB_JVMTI_JVMTI_ENV_PTR_HPP
#define	STATICLIB_JVMTI_JVMTI_ENV_PTR_HPP

#include <functional>
#include <memory>

#include "jni.h"
#include "jvmti.h"

#include "staticlib/config.hpp"
#include "staticlib/jni.hpp"
#include "staticlib/support.hpp"

#include "staticlib/jvmti/jvmti_exception.hpp"

namespace staticlib {
namespace jvmti {

class jvmti_env_ptr {
    std::unique_ptr<jvmtiEnv, std::function<void(jvmtiEnv*)>> jvmti;

public:
    jvmti_env_ptr() :
    jvmti([] {
        sl::jni::error_checker ec;
        jvmtiEnv* env;
        ec = sl::jni::static_java_vm()->GetEnv(reinterpret_cast<void**> (std::addressof(env)), JVMTI_VERSION);
        return std::unique_ptr<jvmtiEnv, std::function<void(jvmtiEnv*)>>(env, [](jvmtiEnv* ptr) {
            if (sl::jni::static_java_vm().init_complete()) {
                auto scoped_jni = sl::jni::thread_local_jni_env_ptr();
                // lets crash early on error
                error_checker ec;
                ec = ptr->DisposeEnvironment();
            } // AttachCurrentThread will crash if init is interrupted
        });
    }()) { }

    jvmti_env_ptr(const jvmti_env_ptr&) = delete;

    jvmti_env_ptr& operator=(const jvmti_env_ptr&) = delete;

    jvmti_env_ptr(jvmti_env_ptr&& other) :
    jvmti(std::move(other.jvmti)) { }

    jvmti_env_ptr& operator=(jvmti_env_ptr&& other) {
        jvmti = std::move(other.jvmti);
        return *this;
    }

    jvmtiEnv* operator->() {
        return jvmti.get();
    }

    jvmtiEnv* get() {
        return jvmti.get();
    }
};

} // namespace
}

#endif	/* STATICLIB_JVMTI_JVMTI_ENV_PTR_HPP */

