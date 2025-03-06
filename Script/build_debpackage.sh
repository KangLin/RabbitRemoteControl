#!/bin/bash
# Author: Kang Lin <kl222@126.com>

#See: https://blog.csdn.net/alwaysbefine/article/details/114187380
#set -x
set -e
#set -v

# store repo root as variable
REPO_ROOT=$(readlink -f $(dirname $(dirname $(readlink -f $0))))
OLD_CWD=$(readlink -f .)

pushd "$REPO_ROOT"

if [ -n "$1" -a -z "$QT_ROOT" ]; then
	QT_ROOT=$1
fi

if [ -d "/usr/lib/`uname -m`-linux-gnu" -a -z "$QT_ROOT" ]; then
    QT_ROOT="/usr/lib/`uname -m`-linux-gnu"
fi

if [ -z "$QT_ROOT" ]; then
    echo "QT_ROOT=$QT_ROOT"
	echo "$0 QT_ROOT RabbitCommon_ROOT"
    exit 1
fi

if [ -n "$2" -a -z "$RabbitCommon_ROOT" ]; then
	RabbitCommon_ROOT=$2
fi

if [ -z "$RabbitCommon_ROOT" ]; then
	RabbitCommon_ROOT=`pwd`/../RabbitCommon
    echo "RabbitCommon_ROOT=$RabbitCommon_ROOT"
fi

if [ ! -d "$RabbitCommon_ROOT" ]; then
    echo "QT_ROOT=$QT_ROOT"
    echo "RabbitCommon_ROOT=$RabbitCommon_ROOT"
	echo "$0 QT_ROOT RabbitCommon_ROOT"
    exit 2
fi

export QT_ROOT=$QT_ROOT
export RabbitCommon_ROOT=$RabbitCommon_ROOT

if [ -z "$BUILD_TYPE" ]; then
    export BUILD_TYPE=Release
fi

export PATH=$QT_ROOT/bin:$PATH

#fakeroot debian/rules binary

# -p, --sign-command=sign-command
#  When dpkg-buildpackage needs to execute GPG to sign a source
#  control (.dsc) file or a .changes file it will run sign-command
#  (searching the PATH if necessary) instead of gpg (long option since
#  dpkg 1.18.8).  sign-command will get all the arguments that gpg
#  would have gotten. sign-command should not contain spaces or any
#  other shell metacharacters.

# -k, --sign-key=key-id
#  Specify a key-ID to use when signing packages (long option since
#  dpkg 1.18.8).

# -us, --unsigned-source
#  Do not sign the source package (long option since dpkg 1.18.8).

# -ui, --unsigned-buildinfo
#  Do not sign the .buildinfo file (since dpkg 1.18.19).

# -uc, --unsigned-changes
#  Do not sign the .buildinfo and .changes files (long option since
#  dpkg 1.18.8).

# -b  Equivalent to --build=binary or --build=any,all.
# -S  Equivalent to --build=source
# -d, --no-check-builddeps    do not check build dependencies and conflicts.
#      --ignore-builtin-builddeps
#                              do not check builtin build dependencies.

#The -us -uc tell it there is no need to GPG sign the package. the -b is build binary
dpkg-buildpackage -us -uc -b -d

#The -us -uc tell it there is no need to GPG sign the package. the -S is build source package
#dpkg-buildpackage -us -uc -S

#dpkg-buildpackage -S

# build source and binary package
#dpkg-buildpackage -us -uc 

#dpkg-buildpackage -b

popd
