#!/bin/bash

###############################################################################
# build.bash
#
# Smart multipurpose C++ library.
#
# Smart build auxiliary script.
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# 2009, (c) Dmitry Timoshenko

# At first check the script location
# and go there if we are not at the script directory
SCRIPT_PATH="${BASH_SOURCE[0]}"
if ([ -h "${SCRIPT_PATH}" ]) then
  while ([ -h "${SCRIPT_PATH}" ]) do
    SCRIPT_PATH=$(readlink "${SCRIPT_PATH}")
  done
fi

#pushd $(dirname "${SCRIPT_PATH}")
pushd . > /dev/null
cd $(dirname "${SCRIPT_PATH}") > /dev/null

cd ./build

if [ "${?}" -ne 0 ]; then
  echo -e "\nSmart building failed"
  exit 1
fi

for param
do
  case "${param}" in
    -help | --help | -h)
    WANT_HELP=true
      ;;

    --use-pre-built-boost=*)
    export SMART_USE_PREBUILT_BOOST=true
      ;;
  esac
done

if [[ "${WANT_HELP}" = "true" ]]; then
  cat <<EOF
Usage: ${0} [OPTION]

To succesfully build unicomm it's neccessary to provide information about
dependencies location on build machine. Assign correct values to
following variables:
  BOOST_ROOT only in case of source tree boost usage
  BOOST_INCLUDE_PATH only in case of pre-built boost usage
  BOOST_LIBRARY_PATH only in case of pre-built boost usage
  XERCESC_ROOT if you are intended to build smart_complex library

or you may use appropriate files in
SMART_ROOT/build/dependencies directory.

Invoked without any parameter causes release in
static linkage configurations will be built.
To build all possible variants use ./build_unicomm.sh script.
You are also enabled to specify what configuration you need exactly.

NOTE: All the parameters specified are just passed to boost bjam engine.
      So you are able to use any syntax bjam allows.
      By default, on Windows platforms msvc toolset is used. On Linux platform
      gcc is used if not specified explicitly.

Something like this:
  ./build.sh variant=debug,release toolset=gcc

The options with no '--' prefix are bjam options and can be used
in different forms like

  ./build.sh debug release

'variant' is ommited in last line.
More details available in boost.build's documentation.

Defaults for the options are specified in brackets if there are.

Configuration:

  -h, --help                Display this help and exit

  --use-pre-built-boost=<BOOST_VER>
                            Designates to use pre-built boost libraries
                            instead of using boost source tree. You should
                            define either BOOST_ROOT or BOOST_INCLUDE_PATH
                            in couple with BOOST_LIBRARY_PATH depending on
                            the option presence.
                            To make the boost build to find pebuilt libraries
                            it's necessary to specify exact version of boost
                            e.g. './build.sh --use-pre-built-boost=1.46.1' will
                            use 1.46.1 to resolve the names.

  --clean                   Cleans the given configuration. This will remove smart
                            related targets within the configuration specified.
                            The targets declared by dependencies (e.g. boost) are not
                            affected. For instance './build.sh --clean' removes
                            all the destination files regarding to 'variant=release' and
                            'link=static,shared' i.e. removes the default configuration's
                            main targets.

  --clean-all               Cleans the given configuration and all it's dependencies.
                            This will remove smart related targets within the
                            configuration specified and all the dependencies are built
                            unlike '--clean' which removes only the smart related targets.

  toolset=<TOOLSET>         Use given toolset.
                            The toolset identifier string can be one of
                            gcc|msvc|intel. There are only
                            gcc and msvc are guaranteed to work
                            properly with smart.
                            [automatically detected]

  define=<MACRO>            Define the specified macro.
                            Please, see smart manuals for macroses
                            being used by smart.

  variant=<VARIANT>         Select one or more the build variants.
                            debug|release are possible.
                            [release]

EOF
fi
test -n "${WANT_HELP}" && exit 0

# Start building
echo -e "\n/////////////////////////////////////////////////////////////////////////"
echo "Start prepearing environment to build smart..."
echo "${@}"

./build.bash "${@}"

popd  > /dev/null
