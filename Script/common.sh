#!/bin/bash

# ==================== ERROR SYMBOLS REFERENCE ====================

# ❌ - Critical error / Failure
# ⚠️  - Warning / Non-critical issue
# ✅ - Success / Completed
# ℹ️  - Information
# 🔧 - Configuration / Setup
# 🚀 - Deployment / Starting process
# 📦 - Package / Installation
# 🔍 - Searching / Checking
# 💡 - Tip / Suggestion
# 🛑 - Stop / Halt execution
# ⏳ - Waiting / In progress
# ✨ - New feature / Enhancement
# 🐛 - Bug / Issue
# 🔒 - Security / Permission issue
# 🌐 - Network / Remote operation
# 📁 - File / Directory operation
# 🔄 - Retry / Loop / Update
# ⚡ - Performance / Quick action
# 📊 - Statistics / Results
# 🎯 - Target / Goal

# ==================== ASCII ERROR INDICATORS ====================

# [ERROR]    or  ERROR:    - Critical error / Failure
# [WARN]     or  WARNING:  - Warning / Non-critical issue
# [OK]       or  SUCCESS:  - Success / Completed
# [INFO]     or  INFO:     - Information
# [CONFIG]   or  CFG:      - Configuration / Setup
# [DEPLOY]   or  DEPLOY:   - Deployment / Starting process
# [CHECK]    or  CHECK:    - Searching / Checking
# [TIP]      or  TIP:      - Suggestion
# [STOP]     or  STOP:     - Halt execution
# [WAIT]     or  WAIT:     - Waiting / In progress
# [SECURITY] or  SEC:      - Security issue

# ASCII Art Symbols (when you want visual indicators)
# [X]        - Error / Failure
# [√]        - Success (if terminal supports)
# [!]        - Warning / Important
# [i]        - Information
# [>]        - Progress / Next step
# [*]        - Generic bullet point

# Validate directory path
validate_directory() {
    local dir="$1"
    local type="$2"

    if [ -n "$dir" ]; then
        if [[ "$dir" =~ ^- ]]; then
            echo "Error: $type directory parameter '$dir' cannot start with '-'" >&2
            exit 1
        fi
    fi
}

# 详细的 Linux 发行版检测函数
get_linux_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        echo "$ID"
    elif [ -f /etc/redhat-release ]; then
        if grep -q "CentOS" /etc/redhat-release; then
            echo "centos"
        elif grep -q "Red Hat" /etc/redhat-release; then
            echo "rhel"
        elif grep -q "Fedora" /etc/redhat-release; then
            echo "fedora"
        else
            echo "redhat"
        fi
    elif [ -f /etc/debian_version ]; then
        echo "debian"
    elif [ -f /etc/arch-release ]; then
        echo "arch"
    elif [ -f /etc/SuSE-release ]; then
        echo "suse"
    elif [ -f /etc/alpine-release ]; then
        echo "alpine"
    else
        echo "unknown"
    fi
}

# 获取版本号
get_linux_version() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        echo "$VERSION_ID"
    elif [ -f /etc/redhat-release ]; then
        grep -oE '[0-9.]+' /etc/redhat-release | head -1
    elif [ -f /etc/debian_version ]; then
        cat /etc/debian_version
    else
        echo "unknown"
    fi
}

# 检测包管理器
get_package_tool() {
    local distro=$1
    case $distro in
        ubuntu|debian|kali)
            echo "apt"
            ;;
        fedora|rhel|centos|almalinux|rocky)
            if command -v dnf >/dev/null 2>&1; then
                echo "dnf"
            else
                echo "yum"
            fi
            ;;
        arch|manjaro)
            echo "pacman"
            ;;
        opensuse*|sles)
            echo "zypper"
            ;;
        alpine)
            echo "apk"
            ;;
        *)
            # 尝试自动检测可用的包管理器
            if command -v apt >/dev/null 2>&1; then
                echo "apt"
            elif command -v dnf >/dev/null 2>&1; then
                echo "dnf"
            elif command -v yum >/dev/null 2>&1; then
                echo "yum"
            elif command -v pacman >/dev/null 2>&1; then
                echo "pacman"
            elif command -v zypper >/dev/null 2>&1; then
                echo "zypper"
            elif command -v apk >/dev/null 2>&1; then
                echo "apk"
            else
                echo "unknown"
            fi
            ;;
    esac
}

package_install() {
    local PACKAGE=$@
    if [ -n "$PACKAGE" ]; then
        #if [ "$BUILD_VERBOSE" = "ON" ]; then
        #    echo "Install package: $PACKAGE"
        #fi
        for p in $PACKAGE
        do
            case "$PACKAGE_TOOL" in
                brew)
                    brew install -q $p
                    ;;
                apt)
                    if [ "$BUILD_VERBOSE" = "ON" ]; then
                        ${PACKAGE_TOOL} install -y -q $p
                    else
                        ${PACKAGE_TOOL} install -y -qq $p
                    fi
                    ;;
                dnf|yum)
                    ${PACKAGE_TOOL} install -y $p
                    ;;
                *)
                    ${PACKAGE_TOOL} install -y $p
                    ;;
            esac
        done
    fi
}

create_debian_folder() {
    local repo_root=$1
    if [ ! -d "$repo_root/debian" ]; then
        echo "Create $repo_root/debian ......"
        ln -s $repo_root/Package/debian $repo_root/debian

        if [ ! -f "$repo_root/debian/control" ]; then
            case "${DISTRO}:${DISTRO_VERSION}" in
                ubuntu:26.*|ubuntu:25.*)
                    control_source="$repo_root/Package/debian/control.ubuntu.26"
                    ;;
                debian:12)
                    control_source="$repo_root/Package/debian/control.debian.12"
                    ;;
                debian:13)
                    control_source="$repo_root/Package/debian/control.debian.13"
                    ;;
                *)
                    control_source="$repo_root/Package/debian/control.default"
                    ;;
            esac
            if [ -f "$control_source" ]; then
                ln -s $control_source $repo_root/debian/control
            else
                echo "Error: $control_source is not exist"
            fi
        fi

    fi
}

install_debian_depend() {
    local repo_root=$1
    create_debian_folder $repo_root
    if [ -f "$repo_root/debian/control" ]; then
        echo "Install deb depends ......"
        pushd $repo_root
        # 安装编译依赖到系统中
        apt-get build-dep -y .
        popd
        # 创建虚拟包来管理依赖， -r : 在编译完成后再删除
        #mk-build-deps -i -r --tool 'apt-get -y' "$repo_root/debian/control"
#        grep-dctrl -s Build-Depends -n . $repo_root/debian/control | \
#            sed 's/([^)]*)//g' | \
#            sed 's/,/\n/g' | \
#            awk '{print $1}' | \
#            grep -v '^$' | \
#            xargs sudo apt install -y
    else
        echo "Error: $repo_root/debian/control is not exist"
    fi
}

# 完整的系统信息函数
get_system_info() {
    local os
    local distro
    local version
    local pkg_tool

    case "$(uname -s)" in
        Darwin*)
            os="macOS"
            distro="macOS"
            version=$(sw_vers -productVersion)
            pkg_tool="brew"
            ;;
        Linux*)
            os="Linux"
            distro=$(get_linux_distro)
            version=$(get_linux_version)
            pkg_tool=$(get_package_tool "$distro")
            ;;
        CYGWIN*|MINGW*|MSYS*)
            os="Windows"
            distro="windows"
            version=$(uname -r)
            pkg_tool="choco"  # Chocolatey
            ;;
        *)
            os="Unknown"
            distro="unknown"
            version="unknown"
            pkg_tool="unknown"
            ;;
    esac

    echo "OS: $os"
    echo "Distribution: $distro"
    echo "Version: $version"
    echo "Package tool: $pkg_tool"
    echo "Architecture: $(uname -m)"
}

install_gnu_getopt() {
    if command -v getopt > /dev/null 2>&1; then
        return
    fi
    case "$DISTRO" in
        fedora)
            package_install util-linux
            ;;
        macOS)
            # 检查　GNU setopt
            # 在 macOS 上，本地 getopt 不支持长格式参数，所以需要先在系统上安装 GNU getopt，并设置环境变量 PATH
            #if [ ! -f /usr/local/opt/gnu-getopt/bin/getopt ]; then
            #    brew install gnu-getopt
            #fi
            #if [[ ! "$PATH" =~ /usr/local/opt/gnu-getopt/bin/getopt/bin ]]; then
            #    export PATH=/usr/local/opt/gnu-getopt/bin/getopt/bin::$PATH
            #fi
            ;;
            *)
            ;;
    esac
}

# macOS 设置函数
setup_macos() {
    echo "Setting up macOS environment..."

    # 检查是否安装了 Homebrew
    if ! command -v brew >/dev/null 2>&1; then
        echo "Installing Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi

    # 确保 Homebrew 在 PATH 中
    if [ -f "/opt/homebrew/bin/brew" ]; then
        eval "$(/opt/homebrew/bin/brew shellenv)"
    elif [ -f "/usr/local/bin/brew" ]; then
        eval "$(/usr/local/bin/brew shellenv)"
    fi

    install_gnu_getopt
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

# 检测操作系统并设置环境变量
detect_os_info() {
    local system_info
    system_info=$(get_system_info)

    export OS=$(echo "$system_info" | grep "^OS:" | cut -d':' -f2 | xargs)
    export DISTRO=$(echo "$system_info" | grep "^Distribution:" | cut -d':' -f2 | xargs)
    export DISTRO_VERSION=$(echo "$system_info" | grep "^Version:" | cut -d':' -f2 | xargs)
    export PACKAGE_TOOL=$(echo "$system_info" | grep "^Package tool:" | cut -d':' -f2 | xargs)
    export ARCHITECTURE=$(echo "$system_info" | grep "^Architecture:" | cut -d':' -f2 | xargs)
#    # 使用 awk 安全地提取值（处理空格）
#    export OS=$(echo "$system_info" | grep "^OS:" | awk -F': ' '{print $2}')
#    export DISTRO=$(echo "$system_info" | grep "^Distribution:" | awk -F': ' '{print $2}')
#    export DISTRO_VERSION=$(echo "$system_info" | grep "^Version:" | awk -F': ' '{print $2}')
#    export PACKAGE_TOOL=$(echo "$system_info" | grep "^Package tool:" | awk -F': ' '{print $2}')
#    export ARCHITECTURE=$(echo "$system_info" | grep "^Architecture:" | awk -F': ' '{print $2}')

    # 如果是 macOS，进行初始化设置
    if [ "$OS" = "macOS" ]; then
        setup_macos
    fi

    if [ "$BUILD_VERBOSE" = "ON" ]; then
        echo "=== OS information (detect_os_info) ==="
        echo "Detected OS:  $OS"
        echo "Distribution: $DISTRO"
        echo "Version:      $DISTRO_VERSION"
        echo "Package tool: $PACKAGE_TOOL"
        echo "Architecture: $ARCHITECTURE"
        echo "======================================="
        echo "PATH: $PATH"
    fi
}

# Simple version comparison function
version_compare() {
    local v1="$1"
    local v2="$2"

    if [[ "$v1" == "$v2" ]]; then
        return 0  # Equal
    fi

    local IFS=.
    local i v1_arr=($v1) v2_arr=($v2)

    for ((i=0; i<${#v1_arr[@]} || i<${#v2_arr[@]}; i++)); do
        local c1=$(printf "%010d" ${v1_arr[i]:-0})
        local c2=$(printf "%010d" ${v2_arr[i]:-0})

        if [[ "$c1" < "$c2" ]]; then
            return 1  # v1 < v2
        elif [[ "$c1" > "$c2" ]]; then
            return 0  # v1 > v2
        fi
    done

    return 0  # Equal
}

# Function to check if git is installed
check_git() {
    if ! command -v git >/dev/null 2>&1; then
        echo "X Git is not installed" >&2
        echo "  Git is required for this script to work." >&2
        echo "" >&2
        echo "  Please install Git using one of the following commands:" >&2

        # Detect OS and suggest installation command
        if [ -f /etc/debian_version ]; then
            echo "   Debian/Ubuntu: sudo apt update && sudo apt install -y git" >&2
        elif [ -f /etc/redhat-release ]; then
            echo "   RHEL/CentOS: sudo yum install -y git" >&2
            echo "   or" >&2
            echo "   Fedora: sudo dnf install -y git" >&2
        elif [ -f /etc/arch-release ]; then
            echo "   Arch Linux: sudo pacman -S git" >&2
        elif [ -f /etc/alpine-release ]; then
            echo "   Alpine Linux: sudo apk add git" >&2
        elif [[ "$OSTYPE" == "darwin"* ]]; then
            echo "   macOS (Homebrew): brew install git" >&2
            echo "   or download from: https://git-scm.com/download/mac" >&2
        else
            echo "   Please install Git from: https://git-scm.com/downloads" >&2
        fi

        return 1
    fi

    if [ "$1" = "1" ]; then

        # Optional: Check git version if minimum version required
        local min_version="${1:-2.0.0}"
        local git_version=$(git --version | cut -d' ' -f3)

        if ! version_compare "$git_version" "$min_version"; then
            echo "!️ Git version $git_version is installed, but version $min_version or higher is recommended" >&2
            # Continue anyway, just a warning
        else
            echo "√ Git $git_version is installed"
        fi
    fi

    return 0
}

# Option to automatically install git (with user confirmation)
install_git_if_missing() {
    if ! command -v git >/dev/null 2>&1; then
        echo "!️ Git is required but not installed." >&2
        read -p "Would you like to install Git now? (y/n): " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            # Detect OS and install
            if [ -f /etc/debian_version ]; then
                sudo apt update && sudo apt install -y git
            elif [ -f /etc/redhat-release ]; then
                sudo yum install -y git
            elif [ -f /etc/arch-release ]; then
                sudo pacman -S --noconfirm git
            elif [ -f /etc/alpine-release ]; then
                sudo apk add git
            elif [[ "$OSTYPE" == "darwin"* ]]; then
                if command -v brew >/dev/null 2>&1; then
                    brew install git
                else
                    echo "X Homebrew not found. Please install Git manually from https://git-scm.com/download/mac" >&2
                    return 1
                fi
            else
                echo "X Unsupported OS. Please install Git manually." >&2
                return 1
            fi

            # Verify installation
            if command -v git >/dev/null 2>&1; then
                echo "√ Git installed successfully!"
            else
                echo "X Git installation failed. Please install manually." >&2
                return 1
            fi
        else
            echo "X Git is required. Exiting." >&2
            return 1
        fi
    fi
    return 0
}

# Function to create sed-safe pattern
sed_safe_pattern() {
    local pattern="$1"
    # Escape special chars for sed BRE
    echo "$pattern" | sed 's/\([][\.*^$+?(){}|/]\)/\\\1/g'
}

# Test function for sed pattern
test_sed_pattern() {
    local test_string="$1"
    local expected="$2"

    echo "Testing: $test_string"

    # Test with -E flag
    if echo "$test_string" | sed -E "s/${VERSION_PATTERN}/REPLACED/g" | grep -q "REPLACED"; then
        echo "  √ -E flag: Matches"
    else
        echo "  X -E flag: No match"
    fi

    # Convert to BRE and test
    BRE_PATTERN=$(sed_safe_pattern "$VERSION_PATTERN")
    if echo "$test_string" | sed "s/${BRE_PATTERN}/REPLACED/g" | grep -q "REPLACED"; then
        echo "  √ BRE: Matches"
    else
        echo "  X BRE: No match"
    fi
}
