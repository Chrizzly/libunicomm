#!/bin/bash

###############################################################################
# build-linux.bash
#
# Smart multipurpose C++ library.
#
# Smart build automation auxiliary script.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# 2009, (c) Dmitry Timoshenko

source ./dependencies.bash

if [ "${?}" -ne 0 ]; then
  echo -e "\nSmart build failed..."
  exit 1
fi

##############################################################################
# Invoke boost build engine

cd ..

bjam "${@}"

if [ "${?}" -ne 0 ]; then
  echo -e "\nSmart build failed..."
  exit 1
fi

echo -e "\nSmart is successfully built!"
