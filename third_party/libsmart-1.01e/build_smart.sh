#!/bin/sh

##############################################################################
# build_all_configurations.bash
#
# Smart multipurpose C++ library.
#
# Smart build automation script. Builds all possible configuration.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# 2009, (c) Dmitry Timoshenko

# Start building
bash ./build.bash variant=debug,release link=static "${@}"
