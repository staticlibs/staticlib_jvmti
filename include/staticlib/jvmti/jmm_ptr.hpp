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

class jmm_ptr {
    // no destruction is required
    sl::support::observer_ptr<JmmInterface> jmm;

public:
    jmm_ptr(std::nullptr_t nothing) :
    jmm(nothing) { }
    
    jmm_ptr() :
    jmm([] {
        auto scoped = sl::jni::thread_local_jni_env_ptr();
        auto res = JVM_GetManagement(JMM_VERSION_1_0);
        if (nullptr == res) {
            throw jvmti_exception(TRACEMSG("Error obtaining JMM interface"));
        }
        return sl::support::observer_ptr<JmmInterface>(static_cast<JmmInterface*>(res));
    }()) { }

    jmm_ptr(const jmm_ptr& other) :
    jmm(other.jmm) { }

    jmm_ptr& operator=(const jmm_ptr& other) {
        jmm = other.jmm;
        return *this;
    }

    JmmInterface* operator->() {
        return jmm.get();
    }

    JmmInterface* get() {
        return jmm.get();
    }
};

} // namespace
}

#endif	/* STATICLIB_JVMTI_JMM_PTR_HPP */

