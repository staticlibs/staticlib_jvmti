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

