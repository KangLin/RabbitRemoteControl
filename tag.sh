#!/bin/bash
set -e

SOURCE_DIR=`pwd`

if [ -n "$1" ]; then
    VERSION=`git describe --tags`
    if [ -z "$VERSION" ]; then
        VERSION=` git rev-parse HEAD`
    fi
    
    echo "Current verion: $VERSION, The version to will be set: $1"
    echo "Please check the follow list:"
    echo "    - Test is ok ?"
    echo "    - Translation is ok ?"
    echo "    - Setup file is ok ?"
    echo "    - Update_*.xml is ok ?"
    
    read -t 30 -p "Be sure to input Y, not input N: " INPUT
    if [ "$INPUT" != "Y" -a "$INPUT" != "y" ]; then
        exit 0
    fi
    git tag -a $1 -m "Release $1"
fi

VERSION=`git describe --tags`
if [ -z "$VERSION" ]; then
    VERSION=`git rev-parse --short HEAD`
fi

sed -i "s/^\!define PRODUCT_VERSION.*/\!define PRODUCT_VERSION \"${VERSION}\"/g" ${SOURCE_DIR}/Install/Install.nsi
sed -i "s/^\SET(BUILD_VERSION.*/\SET(BUILD_VERSION \"${VERSION}\")/g" ${SOURCE_DIR}/CMakeLists.txt
sed -i "s/<VERSION>.*</<VERSION>${VERSION}</g" ${SOURCE_DIR}/Update/update.xml
sed -i "s/export VERSION=.*/export VERSION=\"${VERSION}\"/g" ${SOURCE_DIR}/ci/build.sh
sed -i "s/version:.*/version: \"${VERSION}.{build}\"/g" ${SOURCE_DIR}/appveyor.yml
#sed -i "s/export VERSION=.*/export VERSION=\"${VERSION}\"/g" ${SOURCE_DIR}/.travis.yml
sed -i "s/tag_name:.*/tag_name: '${VERSION}'/g" ${SOURCE_DIR}/.github/workflows/cmake_ubuntu.yml
sed -i "s/tag_name:.*/tag_name: '${VERSION}'/g" ${SOURCE_DIR}/.github/workflows/cmake_mingw.yml
sed -i "s/tag_name:.*/tag_name: '${VERSION}'/g" ${SOURCE_DIR}/.github/workflows/cmake.yml
sed -i "s/RabbitRemoteControl-Setup-v[0-9]\+\.[0-9]\+\.[0-9]\+.exe/RabbitRemoteControl-Setup-${VERSION}.exe/g" ${SOURCE_DIR}/.github/workflows/cmake.yml
sed -i "s/-m \"v[0-9]\+\.[0-9]\+\.[0-9]\+\"/-m \"${VERSION}\"/g" ${SOURCE_DIR}/.github/workflows/cmake.yml
sed -i "s/-m \"v[0-9]\+\.[0-9]\+\.[0-9]\+\"/-m \"${VERSION}\"/g" ${SOURCE_DIR}/.github/workflows/cmake_ubuntu.yml

sed -i "s/^\Standards-Version:.*/\Standards-Version:\"${VERSION}\"/g" ${SOURCE_DIR}/debian/control
DEBIAN_VERSION=`echo ${VERSION}|cut -d "v" -f 2`
sed -i "s/rabbitremotecontrol (.*)/rabbitremotecontrol (${DEBIAN_VERSION})/g" ${SOURCE_DIR}/debian/changelog
sed -i "s/Version=.*/Version=${DEBIAN_VERSION}/g" ${SOURCE_DIR}/debian/RabbitRemoteControl.desktop
sed -i "s/[0-9]\+\.[0-9]\+\.[0-9]\+/${DEBIAN_VERSION}/g" ${SOURCE_DIR}/README*.md
#sed -i "s/[0-9]\+\.[0-9]\+\.[0-9]\+/${DEBIAN_VERSION}/g" ${SOURCE_DIR}/App/android/AndroidManifest.xml
sed -i "s/rabbitremotecontrol_[0-9]\+\.[0-9]\+\.[0-9]\+/rabbitremotecontrol_${DEBIAN_VERSION}/g" ${SOURCE_DIR}/.github/workflows/cmake_ubuntu.yml

MAJOR_VERSION=`echo ${DEBIAN_VERSION}|cut -d "." -f 1`
#sed -i "s/android:versionCode=.*android/android:versionCode=\"${MAJOR_VERSION}\" android/g" ${SOURCE_DIR}/App/android/AndroidManifest.xml

if [ -n "$1" ]; then
    git add .
    git commit -m "Release $1"
    git push
    git tag -d $1
    git tag -a $1 -m "Release $1"
    git push origin :refs/tags/$1
    git push origin $1
fi
