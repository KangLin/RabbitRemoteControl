#!/bin/bash

set -e

update_verion() {

    $SED_CMD "s/^\!define PRODUCT_VERSION.*/\!define PRODUCT_VERSION \"${VERSION}\"/g" ${SOURCE_DIR}/Package/Windows.nsi
    #$SED_CMD "s/export VERSION=.*/export VERSION=\"${VERSION}\"/g" ${SOURCE_DIR}/ci/build.sh
    #$SED_CMD "s/version:.*/version: \"${VERSION}.{build}\"/g" ${SOURCE_DIR}/appveyor.yml
    #$SED_CMD "s/RabbitRemoteControlVersion:.*/RabbitRemoteControlVersion: ${VERSION}/g" ${SOURCE_DIR}/appveyor.yml
    $SED_CMD "s/v${VERSION_PATTERN}/${VERSION}/g" ${SOURCE_DIR}/docs/Doxygen/Develop*.md
    $SED_CMD "s/RabbitRemoteControl_VERSION:.*/RabbitRemoteControl_VERSION: ${VERSION}/g" ${SOURCE_DIR}/.github/workflows/msvc.yml
    $SED_CMD "s/RabbitRemoteControl_VERSION:.*/RabbitRemoteControl_VERSION: ${VERSION}/g" ${SOURCE_DIR}/.github/workflows/mingw.yml
    $SED_CMD "s/RabbitRemoteControl_VERSION:.*/RabbitRemoteControl_VERSION: ${VERSION}/g" ${SOURCE_DIR}/.github/workflows/android.yml
    $SED_CMD "s/RabbitRemoteControl_VERSION:.*/RabbitRemoteControl_VERSION: ${VERSION}/g" ${SOURCE_DIR}/.github/workflows/doxygen.yml
    $SED_CMD "s/RabbitRemoteControl_VERSION:.*/RabbitRemoteControl_VERSION: ${VERSION}/g" ${SOURCE_DIR}/.github/workflows/macos.yml
    $SED_CMD "s/RabbitRemoteControl_VERSION:.*/RabbitRemoteControl_VERSION: ${VERSION}/g" ${SOURCE_DIR}/.github/workflows/macos_script.yml
    $SED_CMD "s/RabbitRemoteControl_VERSION:.*/RabbitRemoteControl_VERSION: ${VERSION}/g" ${SOURCE_DIR}/.github/workflows/appimage.yml
    $SED_CMD "s/RabbitRemoteControl_VERSION:.*/RabbitRemoteControl_VERSION: ${VERSION}/g" ${SOURCE_DIR}/.github/workflows/flatpak.yml
    $SED_CMD "s/v${VERSION_PATTERN}/${VERSION}/g" ${SOURCE_DIR}/README*.md
    $SED_CMD "s/<VERSION>.*</<VERSION>${VERSION}</g" ${SOURCE_DIR}/Update/update.xml
    $SED_CMD "s/          \"version\":[[:blank:]]*\"v\?${VERSION_PATTERN}\"/          \"version\":\"${VERSION}\"/g" ${SOURCE_DIR}/Update/update.json

    $SED_CMD "s/SET(RabbitRemoteControl_VERSION .*)/SET(RabbitRemoteControl_VERSION \"${DEBIAN_VERSION}\")/g" ${SOURCE_DIR}/CMakeLists.txt
    $SED_CMD "s/SET(RabbitRemoteControlTag .*)/SET(RabbitRemoteControlTag \"${DEBIAN_VERSION}\")/g" ${SOURCE_DIR}/CMakeLists.txt

    $SED_CMD "s/RabbitRemoteControl_VERSION:.*/RabbitRemoteControl_VERSION: ${DEBIAN_VERSION}/g" ${SOURCE_DIR}/.github/workflows/build.yml
    $SED_CMD "s/RabbitRemoteControl_VERSION_PRE:.*/RabbitRemoteControl_VERSION_PRE: ${PRE_TAG}/g" ${SOURCE_DIR}/.github/workflows/build.yml
    $SED_CMD "s/version:.*'${VERSION_PATTERN}'/version: '${DEBIAN_VERSION}'/g" ${SOURCE_DIR}/snap/snapcraft.yaml

    RPM_VERSION_PATTERN="[0-9]\+\.[0-9]\+\.[0-9]\+[\+\._~\^0-9A-Za-z]*"
    $SED_CMD "s/Version:.*${RPM_VERSION_PATTERN}/Version:        ${RPM_VERSION}/g" ${SOURCE_DIR}/Package/rpm/rabbitremotecontrol.spec

    CHANGLOG_TMP=${SOURCE_DIR}/Package/debian/changelog.tmp
    CHANGLOG_FILE=${SOURCE_DIR}/Package/debian/changelog
    echo "rabbitremotecontrol (${DEBIAN_VERSION}) unstable; urgency=medium" > ${CHANGLOG_FILE}
    echo "" >> ${CHANGLOG_FILE}
    echo "    * Full Changelog: [${PRE_TAG}..${VERSION}](https://github.com/KangLin/RabbitRemoteControl/compare/${PRE_TAG}..${VERSION})" >> ${CHANGLOG_FILE}
    #echo "`git log --pretty=format:'    * %s' ${PRE_TAG}..HEAD`" >> ${CHANGLOG_FILE}
    echo "" >> ${CHANGLOG_FILE}
    echo " -- `git log --pretty=format:'%an <%ae>' HEAD^..HEAD`  `date --rfc-email`" >> ${CHANGLOG_FILE}

    #CHANGLOG_EXIST="`grep "rabbitremotecontrol (${DEBIAN_VERSION})" ${CHANGLOG_FILE}`"
    #if [ -z "$CHANGLOG_EXIST" ]; then
    #    echo "rabbitremotecontrol (${DEBIAN_VERSION}) unstable; urgency=medium" > ${CHANGLOG_TMP}
    #    echo "" >> ${CHANGLOG_TMP}
    #    echo "`git log --pretty=format:'    * %s (%an <%ae>)' ${PRE_TAG}..HEAD`" >> ${CHANGLOG_TMP}
    #    echo "" >> ${CHANGLOG_TMP}
    #    echo " -- `git log --pretty=format:'%an <%ae>' HEAD^..HEAD`  `date --rfc-email`" >> ${CHANGLOG_TMP}
    #    cat ${CHANGLOG_FILE} >> ${CHANGLOG_TMP}
    #    rm ${CHANGLOG_FILE}
    #    mv ${CHANGLOG_TMP} ${CHANGLOG_FILE}
    #fi

    $SED_CMD "s/RabbitRemoteControl_VERSION:.*/RabbitRemoteControl_VERSION: ${DEBIAN_VERSION}/g" ${SOURCE_DIR}/.github/workflows/ubuntu.yml
    $SED_CMD "s/RabbitRemoteControl_VERSION:.*/RabbitRemoteControl_VERSION: ${DEBIAN_VERSION}/g" ${SOURCE_DIR}/.github/workflows/docker.yml
    $SED_CMD "s/RabbitRemoteControl_VERSION:.*/RabbitRemoteControl_VERSION: ${DEBIAN_VERSION}/g" ${SOURCE_DIR}/.github/workflows/linux.yml
    if [ -f ${SOURCE_DIR}/vcpkg.json ]; then
        $SED_CMD "s/  \"version-string\":.*\"${VERSION_PATTERN}\",/  \"version-string\": \"${DEBIAN_VERSION}\",/g" ${SOURCE_DIR}/vcpkg.json
    fi

    $SED_CMD "s/android:versionCode=\".*\"/android:versionCode=\"${MAJOR_VERSION}\"/g" ${SOURCE_DIR}/App/Client/android/AndroidManifest.xml
    $SED_CMD "s/android:versionName=\"${VERSION_PATTERN}\"/android:versionName=\"${DEBIAN_VERSION}\"/g" ${SOURCE_DIR}/App/Client/android/AndroidManifest.xml

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
        $SED_CMD ":begin; /$START/,/$END/ { /$END/! { $! { N; b begin }; }; s/$START.*$END/$START$CONTENT$END/g ; };" $FILE
    }

    # Generate Authors.md
    #AUTHORS="\n`git log --pretty=format:'- %an' |sort|uniq` \n"
    #replace_paragraph "Contributors" "See:" "$AUTHORS" ${SOURCE_DIR}/Authors.md
    #replace_paragraph "贡献者" "详见" "Author" ${SOURCE_DIR}/Authors_zh_CN.md

}

init_value() {
    SOURCE_DIR=$(dirname $(safe_readlink $0))
    if [ -f ${SOURCE_DIR}/Script/common.sh ]; then
        source ${SOURCE_DIR}/Script/common.sh
        check_git
    fi

    CURRENT_VERSION=`git describe --tags`
    if [ -z "$CURRENT_VERSION" ]; then
        CURRENT_VERSION=`git rev-parse HEAD`
    fi

    PRE_TAG=`git tag --sort=-creatordate | head -n 1`

    # Official SemVer 2.0.0 pattern
    SEMVER_PATTERN="v?(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)(-((0|[1-9][0-9]*|[0-9]*[a-zA-Z-][0-9a-zA-Z-]*)(\.(0|[1-9][0-9]*|[0-9]*[a-zA-Z-][0-9a-zA-Z-]*))*))?(\+[0-9a-zA-Z-]+(\.[0-9a-zA-Z-]+)*)?"
    # SemVer, git, deb and rpm version pattern
    VERSION_PATTERN="v?[0-9]+\.[0-9]+\.[0-9]+([-+_~^][0-9A-Za-z.-]*)?"

    COMMIT="OFF"
    DEPLOY="OFF"
    VERSION=""
    MESSAGE=""

    # Check if sed supports -E
    if sed -E 's/a/b/g' 1>/dev/null 2>/dev/null <<< "test"; then
        #echo "Using sed with -E flag"
        SED_CMD="sed -i -E"
    else
        #echo "Falling back to BRE pattern"
        # Convert pattern to BRE
        BRE_PATTERN=$(sed_safe_pattern "$VERSION_PATTERN")
        SED_CMD="sed -i"
        VERSION_PATTERN="$BRE_PATTERN"
    fi
}

show_value() {
    echo ""
    echo "= Configuration:"
    echo "  Commit: ${COMMIT}"
    echo "  Deploy: ${DEPLOY}"
    echo "  Version: ${VERSION:-not specified}"
    echo "  Message: ${MESSAGE:-not specified}"
    echo "  Remaining args: $@"
    echo ""
    echo "  DEBIAN_VERSION: $DEBIAN_VERSION"
    echo "  RPM_VERSION: $RPM_VERSION"
    echo "  MAJOR_VERSION: $MAJOR_VERSION"
    echo "  DATE_TIME_UTC: $DATE_TIME_UTC"
    echo ""
    echo "  CURRENT_VERSION: $CURRENT_VERSION"
    echo "  PRE_TAG: $PRE_TAG"
    echo "  SOURCE_DIR: $SOURCE_DIR"
    echo ""
    echo "  SED_CMD: $SED_CMD"
    echo "  VERSION_PATTERN: $VERSION_PATTERN"
    echo ""
}

# Display detailed usage information
usage_long() {
    cat << EOF
$(basename $0) - Deploy script

Update version and push to remote repository.

Usage: $0 [OPTION ...] [VERSION]

Options:
  Information:
    -h                      Show this help message
    -s                      Show current version

  Version Management:
    -u                      Auto-increment current version
    -v VERSION              Set specific version (e.g., v1.0.0, 2.1.0-beta)
    -m MESSAGE              Set release message (used with -u, -c, -t)

  Version + Git Operations:
    -c                      Update version and commit changes
    -t VERSION              Set version, create git tag, and push to remote

Version format:
  Follows Semantic Versioning (SemVer) 2.0.0
    See: https://semver.org/

  Format: [v]X.Y.Z[-prerelease][+build]

  Examples:
    v1.0.0               # Release version
    v2.0.0-dev           # Development Version
    2.1.0-beta           # Prerelease version
    v1.2.3-alpha+001     # With build metadata
    3.0.0-rc.1           # Release candidate

Examples:
  # Show current version
  $0
  $0 -s

  # Only update version with current version
  $0 -u
  $0 -u -m "Release note"

  # Only update version
  $0 v1.0.0-dev
  $0 -v v1.0.0-dev

  # Update version and commit code
  # With current version
  $0 -c
  $0 -c -m "Release note"
  # With specify Version
  $0 -c v1.0.0-dev
  $0 -c -m "Release note" v1.0.0-dev

  # Update version and deploy
  $0 -t v1.0.0

  # Update version and deploy with tag and message
  $0 -t v1.0.0 -m "Release note"

EOF
    exit 0
}

parse_with_getopts() {

    if [ $# -eq 0 ]; then
        echo "Current version: $CURRENT_VERSION"
        echo ""
        echo "Help:"
        echo "  $0 -h"
        exit 0
    fi

    # 使用 getopts 解析选项
    # "hdv:t:m:" 含义：
    # h: 无参数
    # c: 无参数
    # s: 无参数
    # v: 需要参数（冒号跟在后面）
    # t: 需要参数
    # m: 需要参数
    while getopts "hcusv:t:m:" opt; do
        case $opt in
            h)
                usage_long
                ;;
            s)
                echo "Current version: $CURRENT_VERSION"
                exit 0
                ;;
            u)
                VERSION=$CURRENT_VERSION
                ;;
            c)
                COMMIT="ON"
                ;;
            v)
                VERSION="$OPTARG"
                #echo "Version set to: $VERSION"
                ;;
            t)
                VERSION="$OPTARG"
                DEPLOY="ON"
                COMMIT="ON"
                #echo "Tag set to: $TAG"
                ;;
            m)
                MESSAGE="$OPTARG"
                #echo "Message set to: $MESSAGE"
                ;;
            \?)
                echo "Invalid option: -$OPTARG" >&2
                usage_long
                ;;
            :)
                echo "Option -$OPTARG requires an argument." >&2
                usage_long
                ;;
        esac
    done

    # 移动参数指针，跳过已处理的选项
    shift $((OPTIND-1))

    # 处理剩余的位置参数（作为 VERSION）
    if [ $# -gt 0 ] && [ -z "$VERSION" ]; then
        VERSION="$1"
        #echo "Version from positional argument: $VERSION"
        shift
    fi

    if [ -z "$VERSION" ]; then
        VERSION=$CURRENT_VERSION
    fi

    # 参数验证
    if [ -n "$VERSION" ]; then
        if [[ ! "$VERSION" =~ ^${VERSION_PATTERN}$ ]]; then
            echo "X Invalid SemVer format: $VERSION" >&2
            echo "  Expected format: [v]X.Y.Z[-prerelease][+build]" >&2
            echo "  See: https://semver.org/" >&2
            exit 1
        fi
    fi

    if [ -n "$VERSION" -a -z "$MESSAGE" ]; then
        MESSAGE="Release $VERSION"
    fi

    # Prepare Debian version (remove 'v' prefix)
    DEBIAN_VERSION="${VERSION#v}"
    #DEBIAN_VERSION=${VERSION/#v/}

    RPM_VERSION=${DEBIAN_VERSION//-/\_}

    # Get major version for versionCode
    MAJOR_VERSION=`echo ${DEBIAN_VERSION} | cut -d "." -f 1`

    # Record update time
    DATE_TIME_UTC=$(date -u +"%Y-%m-%d %H:%M:%S (UTC)")
}

# Function to create sed-safe pattern
sed_safe_pattern() {
    local pattern="$1"
    # Escape special chars for sed BRE
    echo "$pattern" | sed 's/\([][\.*^$+?(){}|/]\)/\\\1/g'
}

# 安全的 readlink 函数，兼容各种系统
safe_readlink() {
    local path="$1"
    if [ -L "$path" ]; then
        if command -v readlink >/dev/null 2>&1; then
            if readlink -f "$path" >/dev/null 2>&1; then
                readlink -f "$path"
            else
                readlink "$path"
            fi
        else
            ls -l "$path" | awk '{print $NF}'
        fi
    elif [ -e "$path" ]; then
        if command -v realpath >/dev/null 2>&1; then
            realpath "$path"
        else
            echo "$(cd "$(dirname "$path")" && pwd)/$(basename "$path")"
        fi
    else
        echo "$path"
    fi
}

create_tag() {
    if [ "$DEPLOY" = "ON" ]; then
        #PRE_TAG=`git tag --sort=-taggerdate | head -n 1`
        echo "= Current version: $CURRENT_VERSION"
        echo "  Latest tag: ${PRE_TAG:-none}"
        echo "  New tag version: $VERSION"
        echo "  Message: $MESSAGE"
        echo ""
        echo "Please verify:"
        echo "  √ Tests passed?"
        echo "  √ Translations updated?"
        echo "  √ Setup files correct?"
        echo "  √ Update_*.json files updated?"
        echo ""

        read -t 30 -p "? Deploy? (y/N): " INPUT
        if [ "${INPUT:-N}" != "Y" ] && [ "${INPUT:-N}" != "y" ]; then
            echo "X Deployment cancelled"
            exit 0
        fi

        echo ""

        # Remove existing tag if it exists
        if git rev-parse "$VERSION" >/dev/null 2>&1; then
            echo "= Tag $VERSION already exists, deleting ......"
            git tag -d "$VERSION"
            echo "√ Successfully delete tag $VERSION"
            echo ""
        fi

        # Create new tag
        echo "= Creating tag: $VERSION ......"
        git tag -a "$VERSION" -m "${MESSAGE}"
        echo "√ Tag created: $VERSION"
        echo ""
    fi
}

commit_code() {
    echo "= Commit code ......"
    git add .

    # Commit if there are changes
    if ! git diff --cached --quiet; then
        git commit -m "$MESSAGE"
        echo "√ Changes committed"
    else
        echo "X No changes to commit"
        exit 1
    fi
}

push_remote_repository() {
    if [ "$DEPLOY" = "ON" ]; then
        echo "= Push to remote repository ......"

        # Update tag (delete and recreate)
        if git rev-parse "$VERSION" >/dev/null 2>&1; then
            git tag -d "$VERSION"
        fi
        git tag -a "$VERSION" -m "$MESSAGE"

        # Push commits and tags
        git push origin HEAD
        git push origin "$VERSION"

        echo "√ Push to remote repository successfully!"
    fi
}

# Start program
init_value

# 执行解析函数
parse_with_getopts "$@"

show_value

create_tag

echo "= Update version to $VERSION ......"

update_verion

echo "√ Version updated to $VERSION successfully!"
#echo "  Time: $DATE_TIME_UTC"
echo ""

if [ "$COMMIT" = "ON" ]; then
    commit_code
fi

push_remote_repository
