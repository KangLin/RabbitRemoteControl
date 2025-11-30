#!/bin/bash

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
    
    # 检查　GNU setopt
    # 在 macOS 上，本地 getopt 不支持长格式参数，所以需要先在系统上安装 GNU getopt，并设置环境变量 PATH
    if [ ! -f /usr/local/opt/gnu-getopt/bin/getopt ]; then
        brew install gnu-getopt
    fi
    export PATH="/usr/local/opt/gnu-getopt/bin:$PATH"
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
    fi
}
