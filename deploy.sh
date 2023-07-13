#!/bin/bash
# Author: Kang Lin <kl222@126.com>

set -e

SOURCE_DIR=`pwd`
PRE_TAG=`git tag --sort=-creatordate | head -n 1`

if [ -n "$1" ]; then
    VERSION=`git describe --tags`
    if [ -z "$VERSION" ]; then
        VERSION=`git rev-parse HEAD`
    fi

    if [ -n "$2" ]; then
        MESSAGE="Release $1 $2"
    else
        MESSAGE="Release $1"
    fi

    PRE_TAG=`git tag --sort=-taggerdate | head -n 1`
    echo "Current version: $VERSION, current tag: $PRE_TAG. The version to will be set tag version: $1 message: $MESSAGE"
    echo "Please check the follow list:"
    echo "    - Test is ok ?"
    echo "    - Translation is ok ?"
    echo "    - Setup file is ok ?"
    echo "    - Update_*.xml is ok ?"
    
    read -t 30 -p "Be sure to input Y, not input N: " INPUT
    if [ "$INPUT" != "Y" -a "$INPUT" != "y" ]; then
        exit 0
    fi
    git tag -a $1 -m "Release $1 ${MESSAGE}"
else
    echo "Usage: $0 release_version [release_message]"
    echo "   release_version format: [v][0-9].[0-9].[0-9]"
    exit -1
fi

# Modify the version number in the version-related files
VERSION=`git describe --tags`
if [ -z "$VERSION" ]; then
    VERSION=`git rev-parse --short HEAD`
fi

sed -i "s/^\!define PRODUCT_VERSION.*/\!define PRODUCT_VERSION \"${VERSION}\"/g" ${SOURCE_DIR}/Install/Install.nsi
sed -i "s/SET(RabbitRemoteControl_VERSION \"v[0-9]\+\.[0-9]\+\.[0-9]\+\")/SET(RabbitRemoteControl_VERSION \"${VERSION}\")/g" ${SOURCE_DIR}/CMakeLists.txt
sed -i "s/<VERSION>.*</<VERSION>${VERSION}</g" ${SOURCE_DIR}/Update/update.xml
sed -i "s/export VERSION=.*/export VERSION=\"${VERSION}\"/g" ${SOURCE_DIR}/ci/build.sh
sed -i "s/version:.*/version: \"${VERSION}.{build}\"/g" ${SOURCE_DIR}/appveyor.yml
sed -i "s/RabbitRemoteControlVersion:.*/RabbitRemoteControlVersion: ${VERSION}/g" ${SOURCE_DIR}/appveyor.yml
#sed -i "s/PROJECT_NUMBER.*v[0-9]\+\.[0-9]\+\.[0-9]\+/PROJECT_NUMBER = ${VERSION}/g" ${SOURCE_DIR}/Doxyfile.in
sed -i "s/RabbitRemoteControl_VERSION:.*/RabbitRemoteControl_VERSION: ${VERSION}/g" ${SOURCE_DIR}/.github/workflows/msvc.yml
sed -i "s/RabbitRemoteControl_VERSION:.*/RabbitRemoteControl_VERSION: ${VERSION}/g" ${SOURCE_DIR}/.github/workflows/mingw.yml
sed -i "s/RabbitRemoteControl_VERSION:.*/RabbitRemoteControl_VERSION: ${VERSION}/g" ${SOURCE_DIR}/.github/workflows/android.yml
sed -i "s/RabbitRemoteControl_VERSION:.*/RabbitRemoteControl_VERSION: ${VERSION}/g" ${SOURCE_DIR}/.github/workflows/doxygen.yml
sed -i "s/RabbitRemoteControl_VERSION:.*/RabbitRemoteControl_VERSION: ${VERSION}/g" ${SOURCE_DIR}/.github/workflows/build.yml

DEBIAN_VERSION=`echo ${VERSION}|cut -d "v" -f 2`
sed -i "s/^\Standards-Version:.*/\Standards-Version:\"${DEBIAN_VERSION}\"/g" ${SOURCE_DIR}/debian/control

CHANGLOG_TMP=${SOURCE_DIR}/debian/changelog.tmp
CHANGLOG_FILE=${SOURCE_DIR}/debian/changelog
echo "rabbitremotecontrol (${DEBIAN_VERSION}) unstable; urgency=medium" > ${CHANGLOG_FILE}
echo "" >> ${CHANGLOG_FILE}
echo "`git log --pretty=format:'    * %s' ${PRE_TAG}..HEAD`" >> ${CHANGLOG_FILE}
echo "" >> ${CHANGLOG_FILE}
echo " -- `git log --pretty=format:'%an <%ae>' HEAD^..HEAD`  `date --rfc-email`" >> ${CHANGLOG_FILE}
echo "" >> ${CHANGLOG_FILE}

#CHANGLOG_EXIST="`grep "rabbitremotecontrol (${DEBIAN_VERSION})" ${CHANGLOG_FILE}`"
#if [ -z "$CHANGLOG_EXIST" ]; then
#    echo "rabbitremotecontrol (${DEBIAN_VERSION}) unstable; urgency=medium" > ${CHANGLOG_TMP}
#    echo "" >> ${CHANGLOG_TMP}
#    echo "`git log --pretty=format:'    * %s (%an <%ae>)' ${PRE_TAG}..HEAD`" >> ${CHANGLOG_TMP}
#    echo "" >> ${CHANGLOG_TMP}
#    echo " -- `git log --pretty=format:'%an <%ae>' HEAD^..HEAD`  `date --rfc-email`" >> ${CHANGLOG_TMP}
#    echo "" >> ${CHANGLOG_TMP}
#    cat ${CHANGLOG_FILE} >> ${CHANGLOG_TMP}
#    rm ${CHANGLOG_FILE}
#    mv ${CHANGLOG_TMP} ${CHANGLOG_FILE}
#fi

sed -i "s/[0-9]\+\.[0-9]\+\.[0-9]\+/${DEBIAN_VERSION}/g" ${SOURCE_DIR}/README*.md
#sed -i "s/[0-9]\+\.[0-9]\+\.[0-9]\+/${DEBIAN_VERSION}/g" ${SOURCE_DIR}/App/android/AndroidManifest.xml
sed -i "s/RabbitRemoteControl_VERSION:.*/RabbitRemoteControl_VERSION: ${DEBIAN_VERSION}/g" ${SOURCE_DIR}/.github/workflows/ubuntu.yml
if [ -f ${SOURCE_DIR}/vcpkg.json ]; then
    sed -i "s/^\  \"version-string\":[0-9]\+\.[0-9]\+\.[0-9]\+/\  \"version-string\":${DEBIAN_VERSION}/g" ${SOURCE_DIR}/vcpkg.json
fi

MAJOR_VERSION=`echo ${DEBIAN_VERSION}|cut -d "." -f 1`
#sed -i "s/android:versionCode=.*android/android:versionCode=\"${MAJOR_VERSION}\" android/g" ${SOURCE_DIR}/App/android/AndroidManifest.xml

#echo "# $VERSION " > ChangeLogDetail.tmp
#echo "`git log --pretty=format:'- %s (%an <%ae>)' ${PRE_TAG}..HEAD`" >> ChangeLogDetail.tmp
#if [ -f ChangeLogDetail.md ]; then
#    cat ChangeLogDetail.md >> ChangeLogDetail.tmp
#fi
#mv ChangeLogDetail.tmp ChangeLogDetail.md

#https://blog.csdn.net/weixin_30530939/article/details/95830331?spm=1001.2101.3001.6661.1&utm_medium=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1-95830331-blog-114387752.pc_relevant_3mothn_strategy_recovery&depth_1-utm_source=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1-95830331-blog-114387752.pc_relevant_3mothn_strategy_recovery&utm_relevant_index=1
function replace_paragraph {
    START=$1
    END=$2
    CONTENT=$3
    FILE=$4
    sed -i ":begin; /$START/,/$END/ { /$END/! { $! { N; b begin }; }; s/$START.*$END/$START$CONTENT$END/g ; };" $FILE
}

# Generate Authors.md
#AUTHORS="\n`git log --pretty=format:'- %an' |sort|uniq` \n"
#replace_paragraph "Contributors" "See:" "$AUTHORS" ${SOURCE_DIR}/Authors.md
#replace_paragraph "贡献者" "详见" "Author" ${SOURCE_DIR}/Authors_zh_CN.md

if [ -n "$1" ]; then
    git add .
    git commit -m "Release $1"
    git tag -d $1
    git tag -a $1 -m "Release $1"
    #git push origin :refs/tags/$1
    git push
    git push origin $1
fi
