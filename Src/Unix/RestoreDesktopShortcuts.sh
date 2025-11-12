#!/bin/bash

echo "=== Desktop shortcuts restore tools ==="

DESKTOP_ENV=$(echo "$XDG_CURRENT_DESKTOP" | tr '[:upper:]' '[:lower:]')

echo "Check desktop environment: $DESKTOP_ENV"

case $DESKTOP_ENV in
    *gnome*)
        echo "Restore GNOME shortcuts ......"
        # 重置所有 GNOME 快捷键设置
        gsettings reset-recursively org.gnome.desktop.wm.keybindings
        gsettings reset-recursively org.gnome.shell.keybindings
        gsettings reset-recursively org.gnome.settings-daemon.plugins.media-keys
        gsettings reset org.gnome.mutter overlay-key
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
        
    *)
        echo "Unsupported desktop: $DESKTOP_ENV"
        ;;
esac

echo "Restored is finished"
