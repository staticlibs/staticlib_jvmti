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
 * File:   agent_base.hpp
 * Author: alex
 *
 * Created on April 22, 2017, 8:41 PM
 */

#ifndef STATICLIB_JVMTI_AGENT_BASE_HPP
#define	STATICLIB_JVMTI_AGENT_BASE_HPP

#include <jni.h>
#include <jvmti.h>

#include <string>
#include <thread>

#include "staticlib/config.hpp"
#include "staticlib/jni.hpp"
#include "staticlib/support.hpp"

#include "staticlib/jvmti/error_checker.hpp"
#include "staticlib/jvmti/jvmti_env_ptr.hpp"
#include "staticlib/jvmti/jvmti_exception.hpp"

namespace staticlib {
namespace jvmti {

template<typename T> class agent_base {
protected:
    jvmti_env_ptr jvmti;
    std::string options;
    std::thread worker;

    agent_base(JavaVM* javavm, char* options) :
    jvmti(javavm),
    options(nullptr != options ? options : "") {
        register_vminit_callback();
        apply_capabilities();
        // start worker
        this->worker = std::thread([this, javavm] {
            // init global jvm pointer
            sl::jni::static_java_vm(javavm);
            // wait for init
            sl::jni::static_java_vm().await_init_complete();
            // call inheritor
            static_cast<T*> (this)->operator()();
        });
    }

    agent_base(const agent_base&) = delete;

    void operator=(const agent_base&) = delete;

    // virtual is unnecessary

    ~agent_base() STATICLIB_NOEXCEPT {
        sl::jni::static_java_vm().notify_shutdown();
        worker.join();
    }

    std::unique_ptr<jvmtiCapabilities> capabilities() {
        auto caps = sl::support::make_unique<jvmtiCapabilities>();
        std::memset(caps.get(), 0, sizeof (*caps));
        return caps;
    }

private:

    void apply_capabilities() {
        auto caps = static_cast<T*> (this)->capabilities();
        error_checker ec;
        ec = jvmti->AddCapabilities(caps.get());
    }

    void register_vminit_callback() {
        jvmtiEventCallbacks cbs;
        memset(std::addressof(cbs), 0, sizeof (cbs));
        cbs.VMInit = [](jvmtiEnv*, JNIEnv*, jthread) {
            sl::jni::static_java_vm().notify_init_complete();
        };
        error_checker ec;
        ec = jvmti->SetEventCallbacks(std::addressof(cbs), sizeof (cbs));
        ec = jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_INIT, nullptr);
    }
};

} // namespace
}

#endif	/* STATICLIB_JVMTI_AGENT_BASE_HPP */

