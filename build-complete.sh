#!/bin/sh

##############################################################################
# build-complete.sh
#
# Unified Communication protocol C++ library.
#
# Try ./build.sh --help for more details.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# 2009, (c) Dmitry Timoshenko

# Start building
bash ./build.bash variant=debug,debug-ssl,release,release-ssl link=static,shared "${@}"
