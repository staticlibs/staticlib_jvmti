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

#include <string>
#include <thread>

// http://stackoverflow.com/q/4845198/314015
#ifdef STATICLIB_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif // STATICLIB_WINDOWS

#include "jni.h"
#include "jvmti.h"

#include "staticlib/config.hpp"
#include "staticlib/jni.hpp"
#include "staticlib/support.hpp"

#include "staticlib/jvmti/error_checker.hpp"
#include "staticlib/jvmti/jvmti_env_ptr.hpp"
#include "staticlib/jvmti/jmm_ptr.hpp"
#include "staticlib/jvmti/jvmti_exception.hpp"

namespace staticlib {
namespace jvmti {

/**
 * Base class template for JVMTI agents. Needs to be inherited using CRTP.
 * Inheritor needs to implement `operator()` and optionally `capabilities()` method.
 * `operator()` will be called after full JVM initialization from spawned thread
 * that is already attached to JVM. On shutdow all threads, possibly spawned by inheritor,
 * must be joined before exiting `operator()` method.
 */
template<typename T> class agent_base {
    bool early_init;
    
protected:
    /**
     * JVMTI environment
     */
    jvmti_env_ptr jvmti;
    /**
     * JMM pointer
     */
    jmm_ptr jmm;
    /**
     * Input options passed to agent
     */
    std::string options;
    /**
     * Thread that is used to invoke `operator()`
     */
    std::thread worker;

    /**
     * Constructor
     * 
     * @param javavm pointer to JVM
     * @param options input options passed to agent
     */
    agent_base(JavaVM* javavm, char* options) :
    early_init([javavm]{
        // init global jvm pointer as early as possible
        sl::jni::static_java_vm(javavm);
        return true;
    }()),
    jvmti(),
    jmm(nullptr),
    options(nullptr != options ? options : "") {
        // init global jvm pointer        
        register_vminit_callback();
        apply_capabilities();
        // start worker
        this->worker = std::thread([this] {
            // wait for init
            sl::jni::static_java_vm().await_init_complete();
            if (sl::jni::static_java_vm().init_complete()) {
                // attach current thread to JVM
                // see: https://bugs.openjdk.java.net/browse/JDK-6404306?focusedCommentId=12313688&page=com.atlassian.jira.plugin.system.issuetabpanels:comment-tabpanel#comment-12313688
                auto scoped = sl::jni::thread_local_jni_env_ptr();
                // init JMM
                this->jmm = jmm_ptr();
                // call inheritor
                static_cast<T*> (this)->operator()();
            }
        });
    }

    /**
     * Deleted copy constructor
     */
    agent_base(const agent_base&) = delete;

    /**
     * Deleted copy assignment operator
     */    
    void operator=(const agent_base&) = delete;

    
    /**
     * Destructor
     */
    // virtual is unnecessary
    ~agent_base() STATICLIB_NOEXCEPT {
        sl::jni::static_java_vm().notify_shutdown();
        worker.join();
    }

    /**
     * This method returns a set of JVMTI capabilities, required
     * by agent. Returns empty set by default, can be overridden 
     * by agent.
     * 
     * @return set of required JVMTI capabilities
     */
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

