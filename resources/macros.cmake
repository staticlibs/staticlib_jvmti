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

# enhanced version of pkg_check_modules macro with PKG_CONFIG_PATH logic
# PKG_CONFIG_PATH handling through CMAKE_PREFIX_PATH was added in newer versions of CMake
macro ( ${PROJECT_NAME}_pkg_check_modules _out_var_name _modifier _modules_list_var_name )
    find_package ( PkgConfig )
    if ( WIN32 )
        set ( PATHENV_SEPARATOR ";" )
    else ( )
        set ( PATHENV_SEPARATOR ":" )
    endif ( )
    set (_pkgconfig_path $ENV{PKG_CONFIG_PATH} )
    if ( STATICLIB_USE_DEPLIBS_CACHE )
        set ( ENV{PKG_CONFIG_PATH} "${STATICLIB_DEPLIBS_CACHE_DIR}/pkgconfig${PATHENV_SEPARATOR}$ENV{PKG_CONFIG_PATH}" )
    endif ( )
    set ( ENV{PKG_CONFIG_PATH} "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/pkgconfig${PATHENV_SEPARATOR}$ENV{PKG_CONFIG_PATH}" )
    pkg_check_modules ( ${_out_var_name} ${_modifier} ${${_modules_list_var_name}} )
    set ( ENV{PKG_CONFIG_PATH} ${_pkgconfig_path} )
endmacro ( )

# converts list to space-separated string with a prefix to each element
macro ( ${PROJECT_NAME}_list_to_string _out_var_name _prefix _list_var_name )
    set ( ${_out_var_name} "" )
    foreach ( _el ${${_list_var_name}} )
        set ( ${_out_var_name} "${${_out_var_name}}${_prefix}${_el} " )
    endforeach ( )
endmacro ( )

# call add_subdirectory using only if specified module is not yet added to main project
macro ( ${PROJECT_NAME}_add_subdirectory _project_path )
    string ( REGEX REPLACE "^.*/" "" _target_name ${_project_path} )
    if ( NOT TARGET ${_target_name} )
        message ( STATUS "Adding dependency: [${_target_name}], path: [${_project_path}]" )
        add_subdirectory ( ${_project_path} ${CMAKE_BINARY_DIR}/${_target_name} )
        set_target_properties ( ${_target_name} PROPERTIES FOLDER "deps" )
    endif ( )
endmacro ( )
