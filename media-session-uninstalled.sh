#!/usr/bin/env bash

set -e

# This is unset by meson
# shellcheck disable=SC2157
if [ -z "@MESON@" ]; then
	SOURCEDIR="@MESON_SOURCE_ROOT@"
	BUILDDIR="@MESON_BUILD_ROOT@"
else
	SOURCEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
	BUILDDIR=${SOURCEDIR}/builddir
fi

while getopts ":b:v:" opt; do
	case ${opt} in
		b)
			BUILDDIR=${OPTARG}
			;;
		v)
			VERSION=${OPTARG}
			echo "Version: ${VERSION}"
			;;
		\?)
			echo "Invalid option: -${OPTARG}"
			exit 1
			;;
		:)
			echo "Option -${OPTARG} requires an argument"
			exit 1
			;;
	esac
done

if [ ! -d "${BUILDDIR}" ]; then
	echo "Invalid build directory: ${BUILDDIR}"
	exit 1
fi

# the config file read by the daemon
export MEDIA_SESSION_CONFIG_DIR="${BUILDDIR}/media-session.d"
export PATH="${BUILDDIR}/src:${PATH}"

export PW_UNINSTALLED=1
export PKG_CONFIG_PATH="${BUILDDIR}/meson-uninstalled/:${PKG_CONFIG_PATH}"

# FIXME: find a nice, shell-neutral way to specify a prompt
${SHELL}
