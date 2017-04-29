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
 * File:   testagent.cpp
 * Author: alex
 *
 * Created on April 22, 2017, 8:49 PM
 */

#include "staticlib/config.hpp"

#include "staticlib/jvmti/agent_base.hpp"

#include <iostream>

#include "staticlib/jni.hpp"


class testagent : public sl::jvmti::agent_base<testagent> {
public:
    testagent(JavaVM* jvm, char* options) :
    sl::jvmti::agent_base<testagent>(jvm, options) { }

    void operator()() STATICLIB_NOEXCEPT {
        try {
            while (sl::jni::static_java_vm().running()) {
                std::cout << "JVM TI agent is running successfully!" << std::endl;
                sl::jni::static_java_vm().thread_sleep_before_shutdown(std::chrono::milliseconds(1000));
            }
            std::cout << "Shutting down agent" << std::endl;
            // all spawned threads must be joined at this point
        } catch (const std::exception& e) {
            std::cout << TRACEMSG(e.what() + "\nWorker error") << std::endl;
        } catch (...) {
            std::cout << TRACEMSG("Unexpected worker error") << std::endl;
        }
    }
};

testagent* global_agent = nullptr;

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM* jvm, char* options, void* /* reserved */) {
    try {
        global_agent = new testagent(jvm, options);
        return JNI_OK;
    } catch (const std::exception& e) {
        std::cout << TRACEMSG(e.what() + "\nInitialization error") << std::endl;
        return JNI_ERR;
    }
}

JNIEXPORT void JNICALL Agent_OnUnload(JavaVM* /* vm */) {
    delete global_agent;
    std::cout << "Agent shutdown complete" << std::endl;
}
