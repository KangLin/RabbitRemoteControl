#!/bin/bash

echo "=== 桌面快捷键恢复工具 ==="

DESKTOP_ENV=$(echo "$XDG_CURRENT_DESKTOP" | tr '[:upper:]' '[:lower:]')

echo "检测到桌面环境: $DESKTOP_ENV"

case $DESKTOP_ENV in
    *gnome*)
        echo "恢复 GNOME 快捷键..."
        # 重置所有 GNOME 快捷键设置
        gsettings reset-recursively org.gnome.desktop.wm.keybindings
        gsettings reset-recursively org.gnome.shell.keybindings
        gsettings reset-recursively org.gnome.settings-daemon.plugins.media-keys
        gsettings set org.gnome.mutter overlay-key 'Super_L'
        killall -3 gnome-shell
        echo "GNOME 快捷键已恢复"
        ;;
        
    *kde*|*plasma*)
        echo "恢复 KDE 快捷键..."
        # 重置 KDE 全局快捷键
        kwriteconfig5 --file kglobalshortcutsrc --group kwin --key "Window Close" "Alt+F4\tAlt+F4,none,KWin"
        kwriteconfig5 --file kglobalshortcutsrc --group kwin --key "Window Maximize" "Meta+PgUp\tMeta+PgUp,none,KWin"
        kwriteconfig5 --file kwinrc --group ModifierOnlyShortcuts --key Meta "org.kde.kglobalaccel,/component/kwin,,invokeShortcut,Show Desktop Grid"
        kwin_x11 --replace &
        echo "KDE 快捷键已恢复"
        ;;
        
    *)
        echo "不支持的桌面环境: $DESKTOP_ENV"
        echo "请手动检查以下设置:"
        echo "1. 系统设置 -> 键盘快捷键"
        echo "2. 窗口管理器设置"
        echo "3. 工作区切换设置"
        ;;
esac

echo "恢复完成"