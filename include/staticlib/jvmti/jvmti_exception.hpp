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
 * File:   jvmti_exception.hpp
 * Author: alex
 *
 * Created on April 22, 2017, 8:38 PM
 */

#ifndef STATICLIB_JVMTI_JVMTI_EXCEPTION_HPP
#define	STATICLIB_JVMTI_JVMTI_EXCEPTION_HPP

#include "staticlib/support/exception.hpp"

namespace staticlib {
namespace jvmti {

/**
 * Module specific exception
 */
class jvmti_exception : public sl::support::exception {
public:
    /**
     * Default constructor
     */
    jvmti_exception() = default;

    /**
     * Constructor with message
     * 
     * @param msg error message
     */
    jvmti_exception(const std::string& msg) :
    sl::support::exception(msg) { }

};

} //namespace
}

#endif	/* STATICLIB_JVMTI_JVMTI_EXCEPTION_HPP */

