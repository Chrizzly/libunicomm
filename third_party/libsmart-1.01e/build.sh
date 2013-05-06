#!/bin/sh

###############################################################################
# build.sh
# 
# Smart multipurpose C++ library.
# 
# Try ./build.sh --help for more details. 
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at 
# http://www.boost.org/LICENSE_1_0.txt)
# 
# 2011, (c) Dmitry Timoshenko

# At first check the script location
# and go there if we are not at the script directory
SCRIPT_PATH="${0}"
if [ -h "${SCRIPT_PATH}" ]; then
  while [ -h "${SCRIPT_PATH}" ]; do 
    SCRIPT_PATH=$(readlink "${SCRIPT_PATH}") 
  done
fi

CURRENT_DIR=$(pwd)

cd $(dirname "${SCRIPT_PATH}") > /dev/null

if [ "${?}" -ne 0 ]; then
  echo -e "\nSmart building failed"
  exit 1
fi

# Start building
bash ./build.bash "${@}"

cd "${CURRENT_DIR}"

