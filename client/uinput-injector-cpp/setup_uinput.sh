#!/usr/bin/env bash
# 一键启用 uinput、添加 udev 规则并把当前用户加入 input 组（需 root 权限）
set -e
if [ "$(id -u)" -ne 0 ]; then
  echo "请以 root 运行此脚本，或使用 sudo。"
  exit 1
fi

echo "加载 uinput 模块..."
modprobe uinput

UDEV_FILE="/etc/udev/rules.d/99-uinput.rules"
echo "创建 udev 规则: $UDEV_FILE"
cat > "$UDEV_FILE" <<'EOF'
KERNEL=="uinput", MODE="0660", GROUP="input", OPTIONS+="static_node=uinput"
EOF

echo "创建 input 组（如果不存在）"
if ! getent group input >/dev/null; then
  groupadd input || true
fi

# 将指定用户加入 input 组
if [ -z "$SUDO_USER" ]; then
  read -p "请输入要加入 input 组的用户名: " TARGET_USER
else
  TARGET_USER="$SUDO_USER"
fi

usermod -a -G input "$TARGET_USER" || echo "注意: usermod 返回非 0，可能用户已在组中或不存在。"

echo "重载 udev 规则..."
udevadm control --reload-rules
udevadm trigger

echo "完成。请注销并重新登录以使组权限生效，或手动执行 'newgrp input'。"
echo "默认 socket 路径: /tmp/rabbit_uinput.sock，程序二进制请放在 /usr/local/bin 或 /opt/rabbit/"
