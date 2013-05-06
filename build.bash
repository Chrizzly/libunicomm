#!/bin/bash

##############################################################################
# build.bash
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

# At first check the script location
# and go there if we are not at the script directory
SCRIPT_PATH="${BASH_SOURCE[0]}"
if [ -h "${SCRIPT_PATH}" ]; then
  while [ -h "${SCRIPT_PATH}" ]; do
    SCRIPT_PATH=$(readlink "${SCRIPT_PATH}")
  done
fi

#pushd $(dirname "${SCRIPT_PATH}")
pushd . > /dev/null
cd $(dirname "${SCRIPT_PATH}") > /dev/null

cd ./build

if [ "${?}" -ne 0 ]; then
  echo -e "\nUnicomm building failed"
  exit 1
fi

for param
do
  case "${param}" in
    -help | --help | -h)
    WANT_HELP=true
      ;;

    --use-complex-xml | define=UNICOMM_USE_COMPLEX_XML)
    export UNICOMM_USE_COMPLEX_XML=true
      ;;

    --use-pre-built-boost=*)
    export UNICOMM_USE_PREBUILT_BOOST=true
      ;;
  esac
done

if [[ "${WANT_HELP}" = "true" ]]; then
  cat <<EOF
Usage: ${0} [options] [properties] [targets]

To succesfully build unicomm it's neccessary to provide information about
dependencies location on build machine. Assign correct values to
following variables:
  BOOST_ROOT only in case of source tree boost usage
  BOOST_INCLUDE_PATH only in case of pre-built boost usage
  BOOST_LIBRARY_PATH only in case of pre-built boost usage

  XERCESC_ROOT if you are intended to use XML engine with the unicomm

or you may use appropriate files located in
'UNICOMM_ROOT/build/deps' subdirectory.

Invoked without parameters causes release in
static and shared linkage configurations will be built.
To build all possible variants use './build-complete.sh' command.
You are also enabled to specify what configuration you need exactly.

NOTE: All the parameters specified are just passed to boost bjam engine.
      So you are able to use any syntax bjam allows.
      By default, on Windows platforms msvc toolset is used. On Linux platform
      gcc is used if not specified explicitly.

Use something like this:
  ./build.sh variant=debug-ssl,release-ssl link=shared toolset=gcc
  ./build.sh debug link=shared toolset=gcc
  ./build.sh variant=debug,release link=shared toolset=gcc
  ./build-complete.sh toolset=gcc
  ./build-complete.sh

The options with no '--' prefix are bjam options and can be used
in different forms like

  ./build.sh variant=debug variant=release
  ./build.sh variant=debug,release
  ./build.sh debug release

More details available in boost.build's documentation.

Defaults for the options are specified in brackets if there are.

Options:

  -h, --help                Display this help and exit

  --prefix=<PREFIX>         Install unicomm files here. By default is not
                            used. To force the installation to the prefix
                            location specify the option or target
                            'unicomm-install' (see below) explicitly.
                            Binaries are installed into <PREFIX>/lib and
                            headers into <PREFIX>/include/unicomm, i.e.
                            if '--prefix=/usr/local' the libraries will be
                            copyied into '/usr/local/lib' and headers
                            will appear in '/usr/local/include/unicomm'.
                            If 'unicomm-install' target and '--prefix' 
                            option is absent binaries installed into 
                            'UNICOMM_ROOT/lib' subdirectory.
                            [/usr/local].

  --use-pre-built-boost=<BOOST_VER>
                            Designates to use pre-built boost libraries
                            instead of using boost source tree. You should
                            define either BOOST_ROOT or BOOST_INCLUDE_PATH
                            in couple with BOOST_LIBRARY_PATH depending on
                            the option presence.
                            To make the boost build to find pebuilt libraries
                            it's necessary to specify exact version of boost
                            e.g. './build.sh --use-pre-built-boost=1.53'
                            will use 1.53 to resolve the names.

  --clean                   Cleans the given configuration. This will remove
                            unicomm related targets within the configuration
                            specified. The targets declared by dependencies
                            (e.g. boost) are not affected. For instance
                            './build.sh --clean' removes all the destination
                            files regarding to 'variant=release' and
                            'link=static,shared' i.e. removes the default
                            configuration's main targets.

  --clean-all               Cleans the given configuration and all it's
                            dependencies. This will remove unicomm related
                            targets within the configuration specified and all
                            the dependencies are built.

  --use-complex-xml         Makes the XML engine to be available.
                            An alternate way to designate to use XML
                            is to specify 'define=UNICOMM_USE_COMPLEX_XML'
                            parameter in command line.

Properties:                            
                            
  toolset=<TOOLSET>         Use given toolset.
                            The toolset identifier string can be one of
                            gcc|msvc. There are only gcc and msvc are
                            guaranteed to work properly with unicomm.
                            [automatically detected]

  define=<MACRO>            Define the specified macro.
                            Please, see unicomm manuals for macroses
                            being used by unicomm.
                            http://libunicomm.org?page=manuals.

  variant=<VARIANT>         Select one or more build variants.
                            debug|release|debug-ssl|release-ssl are possible.
                            [release]

  link=<LINK>               Whether to build static or shared libraries.
                            Allowed values static|shared.
                            [static,shared]

Targets:

  unicomm-install           Installs unicomm to the prefix location.
  http-install              Build and install http sample.
  term-install              Build and install term sample.
  echo-install              Build and install echo sample.

NOTE: Samples installed to the 'UNICOMM_ROOT/out/samples/boost-build' 
      subdirectory.

EOF
fi
test -n "${WANT_HELP}" && exit 0

# Start building
echo -e "\n/////////////////////////////////////////////////////////////////////////"
echo "Start prepearing environment to build unicomm..."
echo "${@}"

./build.bash "${@}"

popd  > /dev/null
