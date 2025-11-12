#!/bin/bash

set -v
echo "=== Desktop shortcuts disable tools ==="

DESKTOP_ENV=$(echo "$XDG_CURRENT_DESKTOP" | tr '[:upper:]' '[:lower:]')

echo "Check desktop environment: $DESKTOP_ENV"

case $DESKTOP_ENV in
    *gnome*)
        echo "Disable GNOME shortcuts ......"
        # Super 键
        gsettings set org.gnome.mutter overlay-key ""
        # 窗口管理
        gsettings set org.gnome.desktop.wm.keybindings close ['']
        gsettings set org.gnome.desktop.wm.keybindings minimize ['']
        gsettings set org.gnome.desktop.wm.keybindings maximize ['']
        gsettings set org.gnome.desktop.wm.keybindings unmaximize ['']
        gsettings set org.gnome.desktop.wm.keybindings begin-move ['']
        gsettings set org.gnome.desktop.wm.keybindings begin-resize ['']
        gsettings set org.gnome.desktop.wm.keybindings toggle-fullscreen ['']
        gsettings set org.gnome.desktop.wm.keybindings toggle-maximized ['']
        # 工作区
        gsettings set org.gnome.desktop.wm.keybindings switch-to-workspace-left ['']
        gsettings set org.gnome.desktop.wm.keybindings switch-to-workspace-right ['']
        gsettings set org.gnome.desktop.wm.keybindings switch-to-workspace-up ['']
        gsettings set org.gnome.desktop.wm.keybindings move-to-workspace-left ['']
        gsettings set org.gnome.desktop.wm.keybindings move-to-workspace-right ['']
        gsettings set org.gnome.desktop.wm.keybindings move-to-workspace-up ['']
        gsettings set org.gnome.desktop.wm.keybindings move-to-workspace-down ['']
        # 其他系统快捷键
        gsettings set org.gnome.desktop.wm.keybindings panel-main-menu ['']
        gsettings set org.gnome.desktop.wm.keybindings panel-run-dialog ['']
        gsettings set org.gnome.desktop.wm.keybindings switch-applications ['']
        gsettings set org.gnome.desktop.wm.keybindings switch-applications-backward ['']
        gsettings set org.gnome.desktop.wm.keybindings switch-windows ['']
        gsettings set org.gnome.desktop.wm.keybindings switch-windows-backward ['']
        gsettings set org.gnome.desktop.wm.keybindings switch-group ['']
        gsettings set org.gnome.desktop.wm.keybindings switch-group-backward ['']
        
        # 面板和 Shell
        gsettings set org.gnome.shell.keybindings toggle-application-view ['']
        gsettings set org.gnome.shell.keybindings toggle-message-tray ['']
        gsettings set org.gnome.shell.keybindings focus-active-notification ['']
        gsettings set org.gnome.shell.keybindings toggle-overview ['']
        # 媒体键
        gsettings set org.gnome.settings-daemon.plugins.media-keys home ['']
        gsettings set org.gnome.settings-daemon.plugins.media-keys email ['']
        gsettings set org.gnome.settings-daemon.plugins.media-keys www ['']
        gsettings set org.gnome.settings-daemon.plugins.media-keys calculator ['']
        gsettings set org.gnome.settings-daemon.plugins.media-keys screensaver ['']
        gsettings set org.gnome.settings-daemon.plugins.media-keys volume-up ['']
        gsettings set org.gnome.settings-daemon.plugins.media-keys volume-down ['']
        gsettings set org.gnome.settings-daemon.plugins.media-keys volume-mute ['']
        gsettings set org.gnome.settings-daemon.plugins.media-keys next ['']
        gsettings set org.gnome.settings-daemon.plugins.media-keys previous ['']
        gsettings set org.gnome.settings-daemon.plugins.media-keys play ['']
        gsettings set org.gnome.settings-daemon.plugins.media-keys pause ['']
        gsettings set org.gnome.settings-daemon.plugins.media-keys stop ['']
        gsettings set org.gnome.settings-daemon.plugins.media-keys custom-keybindings ['']
        
        #killall -3 gnome-shell
        echo "GNOME shortcuts is disabled"
        ;;
        
    *kde*|*plasma*)
        echo "Disable KDE shortcuts ......"
        kwriteconfig5 --file kwinrc --group ModifierOnlyShortcuts --key Meta ""
        kwin_x11 --replace &
        echo "KDE shortcuts is disabled"
        ;;
        
    *)
        echo "Unsupported desktop: $DESKTOP_ENV"
        ;;
esac

echo "Disable is finished"
