#!/bin/bash

function disableGNOME() {
    for schema in $(gsettings list-schemas | grep -E 'keybindings|media-keys')
    do
        #echo "Schema: $schema"
        for key in $(gsettings list-keys $schema)
        do
            #echo "Schema key: $schema $key"
            if [[ $(gsettings range $schema $key) == "type as" ]]; then
                gsettings set $schema $key "@as []"
            fi
        done
    done
}

function diableGNME1() {
    # Window Management
    gsettings set org.gnome.desktop.wm.keybindings close ['']
    gsettings set org.gnome.desktop.wm.keybindings minimize ['']
    gsettings set org.gnome.desktop.wm.keybindings maximize ['']
    gsettings set org.gnome.desktop.wm.keybindings unmaximize ['']
    gsettings set org.gnome.desktop.wm.keybindings begin-move ['']
    gsettings set org.gnome.desktop.wm.keybindings begin-resize ['']
    gsettings set org.gnome.desktop.wm.keybindings toggle-fullscreen ['']
    gsettings set org.gnome.desktop.wm.keybindings toggle-maximized ['']
    # Workspace
    gsettings set org.gnome.desktop.wm.keybindings switch-to-workspace-left ['']
    gsettings set org.gnome.desktop.wm.keybindings switch-to-workspace-right ['']
    gsettings set org.gnome.desktop.wm.keybindings switch-to-workspace-up ['']
    gsettings set org.gnome.desktop.wm.keybindings move-to-workspace-left ['']
    gsettings set org.gnome.desktop.wm.keybindings move-to-workspace-right ['']
    gsettings set org.gnome.desktop.wm.keybindings move-to-workspace-up ['']
    gsettings set org.gnome.desktop.wm.keybindings move-to-workspace-down ['']
    # Other system shortcuts
    gsettings set org.gnome.desktop.wm.keybindings panel-main-menu ['']
    gsettings set org.gnome.desktop.wm.keybindings panel-run-dialog ['']
    gsettings set org.gnome.desktop.wm.keybindings switch-applications ['']
    gsettings set org.gnome.desktop.wm.keybindings switch-applications-backward ['']
    gsettings set org.gnome.desktop.wm.keybindings switch-windows ['']
    gsettings set org.gnome.desktop.wm.keybindings switch-windows-backward ['']
    gsettings set org.gnome.desktop.wm.keybindings switch-group ['']
    gsettings set org.gnome.desktop.wm.keybindings switch-group-backward ['']

    # Panel and Shell
    gsettings set org.gnome.shell.keybindings toggle-application-view ['']
    gsettings set org.gnome.shell.keybindings toggle-message-tray ['']
    gsettings set org.gnome.shell.keybindings focus-active-notification ['']
    gsettings set org.gnome.shell.keybindings toggle-overview ['']
    # Media keys
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
}

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

function disableXFconf() {
    local BACKUP="${HOME}/.config/xfce4/xfconf/xfwm-shortcuts.RabbitRemoteControl.bak"
    local CHANNEL="xfce4-keyboard-shortcuts"

    if [ ! -f "$BACKUP" ]; then
        if ! xfconf-query -c "$CHANNEL" -lv > "$BACKUP" 2>/dev/null; then
            echo "Failed backup to $BACKUP" >&2
            return 1
        fi
        echo "Backup to $BACKUP"
    fi

    local PROPERTIES=(/xfwm4/custom /xfwm4/default)
    local base prop rc=0
    for base in "${PROPERTIES[@]}"; do
        while read -r prop; do
            [ -z "$prop" ] && continue
            if ! xfconf-query -c "$CHANNEL" -p "$prop" -r 2>/dev/null; then
                echo "Failed to remove $prop" >&2
                rc=1
            fi
        done < <(xfconf-query -c "$CHANNEL" -p "$base" -l 2>/dev/null)
    done
    return $rc
}

function restoreXFconf() {
    local BACKUP="${HOME}/.config/xfce4/xfconf/xfwm-shortcuts.RabbitRemoteControl.bak"
    local CHANNEL="xfce4-keyboard-shortcuts"

    if [ ! -f "$BACKUP" ]; then
        echo "The backup file is not exist: $BACKUP"
        exit 1
    fi

    fail=0
    while IFS= read -r line; do
        [ -z "$line" ] && continue
        # 第一个空白前是属性名，其余是值
        prop="${line%%[$' \t']*}"
        value="${line#"$prop"}"
        value="${value#"${value%%[!$' \t']*}"}"   # 去掉值前面的空白

        if ! xfconf-query -c "$CHANNEL" -p "$prop" -s "$value" --create -t string; then
            echo "Failed: restore $prop"
            fail=1
        fi
    done < "$BACKUP"

    [ $fail -eq 0 ] && echo "Restore finished" || exit 1
}

function disableLXQT() {
    echo "Disable LXQT shortcuts ......"
    if [ -f ${HOME}/.config/lxqt/globalkeyshortcuts.conf ]; then
        # 配置文件是: ${HOME}/.config/lxqt/globalkeyshortcuts.conf
        if [ -f ${HOME}/.config/lxqt/globalkeyshortcuts.conf.RabbitRemoteControl ]; then
            echo "The last startup ended unexpectedly !!!"
        else
            # 备份配置文件
            cp ${HOME}/.config/lxqt/globalkeyshortcuts.conf ${HOME}/.config/lxqt/globalkeyshortcuts.conf.RabbitRemoteControl
        fi
        # 禁用所有快捷键
        sed -i 's/^AllowGrabBaseKeypad=.*/AllowGrabBaseKeypad=false/' "${HOME}/.config/lxqt/globalkeyshortcuts.conf"
        sed -i 's/^AllowGrabBaseSpecial=.*/AllowGrabBaseSpecial=false/' "${HOME}/.config/lxqt/globalkeyshortcuts.conf"
        sed -i 's/^AllowGrabLocks=.*/AllowGrabLocks=false/' "${HOME}/.config/lxqt/globalkeyshortcuts.conf"
        sed -i 's/^AllowGrabMiscKeypad=.*/AllowGrabMiscKeypad=false/' "${HOME}/.config/lxqt/globalkeyshortcuts.conf"
        sed -i 's/^AllowGrabMiscSpecial=.*/AllowGrabMiscSpecial=false/' "${HOME}/.config/lxqt/globalkeyshortcuts.conf"
        sed -i 's/^MultipleActionsBehaviour=.*/MultipleActionsBehaviour=false/' "${HOME}/.config/lxqt/globalkeyshortcuts.conf"
        sed -i 's/^Enabled=.*/Enabled=false/' "${HOME}/.config/lxqt/globalkeyshortcuts.conf"
        # 结束后台快捷键捕获程序
        pkill -f lxqt-globalkeysd
        # 再重新启动（通常它会被会话自动拉起，或手动在后台运行）
        lxqt-globalkeysd &
    else
        echo "The file is not exist: ${HOME}/.config/lxqt/globalkeyshortcuts.conf"
    fi
    disableXFconf
}

function resetLXQT() {
    echo "Restore LXQT shortcuts ......"
    if [ -f ~/.config/lxqt/globalkeyshortcuts.conf.RabbitRemoteControl ]; then
        # 配置文件是: ~/.config/lxqt/globalkeyshortcuts.conf
        # 恢复备份的配置文件
        mv ~/.config/lxqt/globalkeyshortcuts.conf.RabbitRemoteControl ~/.config/lxqt/globalkeyshortcuts.conf
        # 结束后台快捷键捕获程序
        pkill -f lxqt-globalkeysd
        # 再重新启动（通常它会被会话自动拉起，或手动在后台运行）
    lxqt-globalkeysd &
    fi

    restoreXFconf
}

DESKTOP_ENV=$(echo "$XDG_CURRENT_DESKTOP" | tr '[:upper:]' '[:lower:]')
echo "Check desktop environment: $DESKTOP_ENV"