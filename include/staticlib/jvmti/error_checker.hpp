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
 * File:   error_checker.hpp
 * Author: alex
 *
 * Created on April 22, 2017, 8:39 PM
 */

#ifndef STATICLIB_JVMTI_ERROR_CHECKER_HPP
#define	STATICLIB_JVMTI_ERROR_CHECKER_HPP

#include "jni.h"
#include "jvmti.h"

#include "staticlib/config.hpp"
#include "staticlib/support.hpp"

#include "staticlib/jvmti/jvmti_exception.hpp"

namespace staticlib {
namespace jvmti {

class error_checker {
public:
    void operator=(jvmtiError err) {
        if (JVMTI_ERROR_NONE != err) {
            throw jvmti_exception(TRACEMSG("JVMTI error code: [" + sl::support::to_string(err) + "]"));
        }
    }
};

} // namespace
}

#endif	/* STATICLIB_JVMTI_ERROR_CHECKER_HPP */

