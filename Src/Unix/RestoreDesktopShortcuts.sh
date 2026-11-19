#!/bin/bash
# Author: Kang Lin <kl222@126.com>
# Restore desktop shortcuts

#set -x
#set -e
#set -v

echo "=== Desktop shortcuts restore tools ==="

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
    *gnome*|*cinnamon*)
        echo "Restore GNOME shortcuts ......"
        # super key
        gsettings reset org.gnome.mutter overlay-key
        resetGNOME
        #killall -3 gnome-shell
        echo "GNOME is restored"
        ;;

    *kde*|*plasma*)
        echo "Restore KDE shortcuts ......"
        # 重置 KDE 全局快捷键
        kwriteconfig5 --file kglobalshortcutsrc --group kwin --key "Window Close" "Alt+F4\tAlt+F4,none,KWin"
        kwriteconfig5 --file kglobalshortcutsrc --group kwin --key "Window Maximize" "Meta+PgUp\tMeta+PgUp,none,KWin"
        kwriteconfig5 --file kwinrc --group ModifierOnlyShortcuts --key Meta "org.kde.kglobalaccel,/component/kwin,,invokeShortcut,Show Desktop Grid"
        kwin_x11 --replace &
        echo "KDE is restored"
        ;;

    *xfce*)
        restoreXFCE
        ;;

    *lxqt*)
        restoreLXQT
        ;;

    *)
        echo "Unsupported desktop: $DESKTOP_ENV"
        ;;
esac

echo "Restored is finished"
