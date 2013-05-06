#!/bin/bash

##############################################################################
# deps.bash
#
# Unified Communication protocol C++ library.
#
# Sets up the unicomm dependencies.
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

function setup_library_relative_path()
{
  setup_library_root "${1}" "${2}"

  cd ./deps
  eval "${1}"=$(readlink -f -s ${!1})
  cd ..

  check_path "${1}"
  add_trailing_slash "${1}"
}

# dependency location files
xercesc_root_file=./deps/xercesc_root-linux
boost_root_file=./deps/boost_root-linux
smart_root_file=./deps/smart_root-linux
boost_include_path_file=./deps/boost_include_path-linux
boost_library_path_file=./deps/boost_library_path-linux

##############################################################################
# Boost Library configuration
# for details see http://www.boost.org

if [ -z "${UNICOMM_USE_PREBUILT_BOOST}" ]; then
  setup_library "BOOST_ROOT" "${boost_root_file}"
else
  setup_library "BOOST_INCLUDE_PATH" "${boost_include_path_file}"
  setup_library "BOOST_LIBRARY_PATH" "${boost_library_path_file}"
fi

##############################################################################
# Smart Library configuration
# for details see http://smartxx.org
#setup_library "SMART_ROOT" "${smart_root_file}"
setup_library_relative_path "SMART_ROOT" "${smart_root_file}"

##############################################################################
# Xerces-C Library configuration
# for details see http://xerces.apache.org/xerces-c/
if [[ "${UNICOMM_USE_COMPLEX_XML}" == "true" ]]; then
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
fi

echo -e "\nEnvironment is properly set...\n"

if [ -z "${UNICOMM_USE_PREBUILT_BOOST}" ]; then
  echo "Boost:          ${BOOST_ROOT}"
else
  echo "Boost headers:  ${BOOST_INCLUDE_PATH}"
  echo "Boost binaries: ${BOOST_LIBRARY_PATH}"
fi

#echo "Smart:          ${SMART_ROOT}"

if [[ "${UNICOMM_USE_COMPLEX_XML}" == "true" ]]; then
  echo "Xerces-C:       ${XERCESC_ROOT}"
fi

echo -e "\nReady to build unicomm..."

