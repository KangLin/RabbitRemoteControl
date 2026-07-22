RabbitRemoteControl — uinput 注入器 (C++)

目的
- 为了在 Linux（包括 Wayland）会话中注入系统按键（例如 Super/Meta），提供一个基于 uinput 的本地���入器，供 RabbitRemoteControl 的客户端调用。

内容
- uinput_injector.cpp — 主程序，监听 UNIX socket (/tmp/rabbit_uinput.sock)，接受简单文本命令并注入按键事件。
- setup_uinput.sh — 一键安装脚本：加载 uinput 模块、添加 udev 规则并把用户加入 input 组。
- Makefile — 构建脚本。

快速开始
1) 构建
   sudo apt install build-essential   # 如需
   cd client/uinput-injector-cpp
   make

2) 安装并授权（以 root 运行）
   sudo bash setup_uinput.sh

   脚本会：
   - modprobe uinput
   - 在 /etc/udev/rules.d/99-uinput.rules 中添加规则，使 /dev/uinput 可被 input 组访问
   - 把运行程序的用户加入 input 组（需要注销/重新登录生效）

3) 运行注入器（带权限）
   sudo ./uinput_injector
   或把二进制放到 /usr/local/bin 并以 systemd 服务/daemon 运行（推荐以有权限的专用进程运行）

协议（简单文本）
- 客户端通过 UNIX socket 向 /tmp/rabbit_uinput.sock 发送一行命令（带换行）并读取响应：
  - "press KEY_NAME"  或 "PRESS KEY_NAME"
  - "release KEY_NAME" 或 "RELEASE KEY_NAME"
- 常见 KEY_NAME: SUPER, LEFTMETA, A, B, ..., 1,2,..., ENTER, SPACE, TAB, ESC, UP, DOWN, LEFT, RIGHT
- 响应: "OK", "ERR", "UNKNOWN_KEY", "UNKNOWN_ACTION"

安全与注意事项
- uinput 允许注入任意输入事件，存在安全风险。仅在用户明确允许且了解风险的情况下启用。
- 推荐：
  - 仅在受信任的网络/会话启用注入功能
  - 在 RabbitRemoteControl 协议中对注入请求做严格鉴权与授权
  - 将注入器作为本地受限服务运行（systemd + 指定用户）
- Wayland 原生客户端可能对某些事件有不同处理，但 uinput 注入在内核层对所有会话有效。

集成建议
- RabbitRemoteControl 客户端应在用户授权后向本地 socket 发送注入命令（文本或 JSON 可扩展），并在 UI 中显示步骤与安全说明。
- 可扩展为 JSON-RPC、TLS-over-socket 或其它更安全的管道，并限制允许注入的按键/来源。
