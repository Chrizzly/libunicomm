#!/bin/bash

##############################################################################
# deps.bash
#
# Smart multipurpose C++ library.
#
# Sets up smart dependencies.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# 2009, (c) Dmitry Timoshenko

##############################################################################
# Sets specified library root
function setup_library_root()
{
  if [ -z "${!1}" ]; then
    if [ ! -f "${2}" ]; then
      echo -e "\n${1}: File [${2}] that contains library location doesn't exist"
      exit 1
    fi

    eval export "${1}"=$(cat ${2})
  fi
}

##############################################################################
# Checks for an error in the given path and exits if it is
function check_path()
{
  if [ -z "${1}" ]; then
    echo -e "\nInvalid check_path argument: Null reference"
    exit 1
  fi

  if [ -z "${!1}" ]; then
    echo -e "\n${1}: Location is undefined"
    exit 1
  fi

  if [ ! -d "${!1}" ]; then
    echo -e "\n${1}: Specified path [${!1}] doesn't exist"
    exit 1
  fi
}

##############################################################################
# Adds trailing slash if it's necessary
function add_trailing_slash()
{
  local tmp=${!1}
  if [ "${tmp:$((${#tmp} - 1)):1}" != "/"  ]; then
    eval "${1}"="${tmp}"/
  fi
}

##############################################################################
# Sets up a library
function setup_library()
{
  setup_library_root "${1}" "${2}"
  check_path "${1}"
  add_trailing_slash "${1}"
}

# dependency location files
boost_root_file=./deps/boost_root-linux
boost_include_path=./deps/boost_include_path-linux
boost_library_path=./deps/boost_library_path-linux
xercesc_root_file=./deps/xercesc_root-linux

##############################################################################
# Boost Library configuration
# for details see http://www.boost.org
if [ -z "${SMART_USE_PREBUILT_BOOST}" ]; then
  setup_library "BOOST_ROOT" "${boost_root_file}"
else
  setup_library "BOOST_INCLUDE_PATH" "${boost_include_path}"
  setup_library "BOOST_LIBRARY_PATH" "${boost_library_path}"
fi

##############################################################################
# Xerces-C Library configuration
# for details see http://xerces.apache.org/xerces-c/

if [ -n "${XERCESCROOT}" ]; then
  if [ -z "${XERCESC_ROOT}" ]; then
    export XERCESC_ROOT="${XERCESCROOT}"
  fi
else
  # Specify path to the xerces-c root directory
  setup_library_root "XERCESC_ROOT" "${xercesc_root_file}"
fi

check_path "XERCESC_ROOT"

add_trailing_slash "XERCESC_ROOT"

echo -e "\nEnvironment is properly set...\n"

if [ -z "${SMART_USE_PREBUILT_BOOST}" ]; then
  echo "Boost:          ${BOOST_ROOT}"
else
  echo "Boost headers:  ${BOOST_INCLUDE_PATH}"
  echo "Boost binaries: ${BOOST_LIBRARY_PATH}"
fi

echo "Xerces-C:       ${XERCESC_ROOT}"

echo -e "\nReady to build smart..."

