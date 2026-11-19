#!/bin/bash
# Author: Kang Lin <kl222@126.com>
# Disable desktop shortcuts

#set -x
#set -e
#set -v

echo "=== Desktop shortcuts disable tools ==="

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

source $(dirname $(safe_readlink ${BASH_SOURCE[0]}))/DesktopShotsCommon.sh

case $DESKTOP_ENV in
    *gnome*)
        echo "Disable GNOME shortcuts ......"
        # Super
        gsettings set org.gnome.mutter overlay-key ""

        disableGNOME

        #killall -3 gnome-shell
        echo "GNOME shortcuts is disabled"
        ;;
    *kde*|*plasma*)
        echo "Disable KDE shortcuts ......"
        kwriteconfig5 --file kwinrc --group ModifierOnlyShortcuts --key Meta ""
        kwin_x11 --replace &
        echo "KDE shortcuts is disabled"
        ;;
    *xfce*)
        disableXFCE
        ;;
    *lxqt*)
        disableLXQT
        ;;
    *)
        echo "Unsupported desktop: $DESKTOP_ENV"
        ;;
esac

echo "Disable is finished"
