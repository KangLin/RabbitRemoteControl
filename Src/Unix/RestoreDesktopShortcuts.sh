#!/bin/bash

#set -x
#set -e
#set -v

echo "=== Desktop shortcuts restore tools ==="

function resetGNOME() {
    for schema in $(gsettings list-schemas | grep -E 'keybindings|media-keys')
    do
        for key in $(gsettings list-keys $schema)
        do
            if [[ $(gsettings range $schema $key) == "type as" ]]; then
                gsettings reset $schema $key
            fi
        done
    done
}

DESKTOP_ENV=$(echo "$XDG_CURRENT_DESKTOP" | tr '[:upper:]' '[:lower:]')

echo "Check desktop environment: $DESKTOP_ENV"

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
        
    *)
        echo "Unsupported desktop: $DESKTOP_ENV"
        ;;
esac

echo "Restored is finished"
