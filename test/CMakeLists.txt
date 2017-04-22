# Copyright 2017, alex at staticlibs.net
#
# This code is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 only, as
# published by the Free Software Foundation.
#
# This code is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# version 2 for more details (a copy is included in the LICENSE.md file that
# accompanied this code).

cmake_minimum_required ( VERSION 2.8.12 )

# toolchain setup
set ( STATICLIB_TOOLCHAIN linux_amd64_gcc CACHE STRING "toolchain triplet" )
if ( NOT DEFINED STATICLIB_CMAKE )
    set ( STATICLIB_CMAKE ${CMAKE_CURRENT_LIST_DIR}/../../cmake CACHE INTERNAL "" )    
endif ( )
set ( CMAKE_TOOLCHAIN_FILE ${STATICLIB_CMAKE}/toolchains/${STATICLIB_TOOLCHAIN}.cmake CACHE INTERNAL "" )

# project
project ( staticlib_jvmti_test CXX )
include ( ${STATICLIB_CMAKE}/staticlibs_common.cmake )

# options
set ( JAVA_HOME $ENV{JAVA_HOME} CACHE STRING "Path to JDK directory" )

# dependencies
if ( NOT DEFINED STATICLIB_DEPS )
    set ( STATICLIB_DEPS ${CMAKE_CURRENT_LIST_DIR}/../../ CACHE INTERNAL "" )    
endif ( )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_config )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_support )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_concurrent )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_jni )
staticlib_add_subdirectory ( ${CMAKE_CURRENT_LIST_DIR}/../../staticlib_jvmti )
set ( ${PROJECT_NAME}_DEPS staticlib_jvmti )
staticlib_pkg_check_modules ( ${PROJECT_NAME}_DEPS_PC REQUIRED ${PROJECT_NAME}_DEPS )

# library
add_library ( ${PROJECT_NAME} SHARED 
        ${CMAKE_CURRENT_LIST_DIR}/testagent.cpp )
source_group ( "src" FILES ${CMAKE_CURRENT_LIST_DIR}/testagent.cpp )
target_include_directories ( ${PROJECT_NAME} BEFORE PRIVATE
        ${JAVA_HOME}/include
        ${CMAKE_CURRENT_LIST_DIR}
        ${${PROJECT_NAME}_DEPS_PC_INCLUDE_DIRS} )
if ( STATICLIB_TOOLCHAIN MATCHES "linux_[^_]+_[^_]+" )
    target_include_directories ( ${PROJECT_NAME} BEFORE PRIVATE
            ${JAVA_HOME}/include/linux )
elseif ( STATICLIB_TOOLCHAIN MATCHES "windows_[^_]+_[^_]+" )
    target_include_directories ( ${PROJECT_NAME} BEFORE PRIVATE
            ${JAVA_HOME}/include/windows )
elseif ( STATICLIB_TOOLCHAIN MATCHES "macosx_[^_]+_[^_]+" )
    target_include_directories ( ${PROJECT_NAME} BEFORE PRIVATE
            ${JAVA_HOME}/include/macosx )
endif ( )
target_compile_options ( ${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_DEPS_PC_CFLAGS_OTHER} )

enable_testing ( )
set ( ${PROJECT_NAME}_TARGET ${CMAKE_SHARED_LIBRARY_PREFIX}${PROJECT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX} )
message ( STATUS "Compiling test java app" )
execute_process ( 
        COMMAND ${JAVA_HOME}/bin/javac -d . ../test/App.java
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} )
message ( STATUS "Compiling test java app - done" )
add_test ( test ${JAVA_HOME}/bin/java -agentpath:bin/${${PROJECT_NAME}_TARGET} App )
