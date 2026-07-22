/*
  uinput_injector.cpp
  简单的 uinput 注入器（C++），在 /tmp/rabbit_uinput.sock 上监听文本命令：
    press KEY_NAME
    release KEY_NAME
  支持的 KEY_NAME 示例: SUPER, LEFTMETA, A, B, C, 1, 2, ENTER, SPACE, TAB, ESC, UP, DOWN, LEFT, RIGHT
  编译: make
  运行（示例）: sudo ./uinput_injector
  注：通常需要 /dev/uinput 写权限（脚本会提供 udev 规则）。
*/

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <map>
#include <vector>
#include <string>
#include <sys/ioctl.h>
#include <linux/uinput.h>
#include <linux/input.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

static const char *SOCKET_PATH = "/tmp/rabbit_uinput.sock";
int uinput_fd = -1;
int server_fd = -1;

void cleanup_and_exit(int sig) {
    if (server_fd >= 0) {
        close(server_fd);
        unlink(SOCKET_PATH);
    }
    if (uinput_fd >= 0) {
        ioctl(uinput_fd, UI_DEV_DESTROY);
        close(uinput_fd);
    }
    std::cout << "Exiting\n";
    _exit(0);
}

bool setup_uinput_device() {
    uinput_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (uinput_fd < 0) {
        perror("open /dev/uinput");
        return false;
    }

    // Enable key events
    if (ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY) < 0) { perror("UI_SET_EVBIT EV_KEY"); /*continue*/ }
    if (ioctl(uinput_fd, UI_SET_EVBIT, EV_SYN) < 0) { perror("UI_SET_EVBIT EV_SYN"); /*continue*/ }

    // Enable some common keys
    std::vector<int> keys = {
        KEY_LEFTMETA, KEY_RIGHTMETA,
        KEY_LEFTCTRL, KEY_LEFTALT, KEY_LEFTSHIFT,
        KEY_ENTER, KEY_ESC, KEY_SPACE, KEY_TAB, KEY_BACKSPACE,
        KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT
    };
    // letters A-Z
    for (int k = KEY_A; k <= KEY_Z; ++k) keys.push_back(k);
    // numbers 1-9 and 0
    keys.push_back(KEY_1); keys.push_back(KEY_2); keys.push_back(KEY_3);
    keys.push_back(KEY_4); keys.push_back(KEY_5); keys.push_back(KEY_6);
    keys.push_back(KEY_7); keys.push_back(KEY_8); keys.push_back(KEY_9);
    keys.push_back(KEY_0);

    // set bits
    for (int code : keys) {
        if (ioctl(uinput_fd, UI_SET_KEYBIT, code) < 0) {
            // Non-fatal; continue
        }
    }

    struct uinput_user_dev uidev;
    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "rabbit-remote-uinput");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1;
    uidev.id.product = 0x1;
    uidev.id.version = 1;

    if (write(uinput_fd, &uidev, sizeof(uidev)) < 0) { perror("write uidev"); return false; }
    if (ioctl(uinput_fd, UI_DEV_CREATE) < 0) { perror("UI_DEV_CREATE"); return false; }

    // small sleep to let device appear
    sleep(1);
    return true;
}

int emit_event(int fd, int type, int code, int value) {
    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    gettimeofday(&ev.time, NULL);
    ev.type = type;
    ev.code = code;
    ev.value = value;
    if (write(fd, &ev, sizeof(ev)) < 0) {
        perror("write event");
        return -1;
    }
    return 0;
}

int send_key(int fd, int code, bool press) {
    if (emit_event(fd, EV_KEY, code, press ? 1 : 0) < 0) return -1;
    if (emit_event(fd, EV_SYN, SYN_REPORT, 0) < 0) return -1;
    return 0;
}

std::map<std::string,int> build_keymap() {
    std::map<std::string,int> m;
    m["SUPER"] = KEY_LEFTMETA;
    m["LEFTMETA"] = KEY_LEFTMETA;
    m["RIGHTMETA"] = KEY_RIGHTMETA;
    m["CTRL"] = KEY_LEFTCTRL;
    m["ALT"] = KEY_LEFTALT;
    m["SHIFT"] = KEY_LEFTSHIFT;
    m["ENTER"] = KEY_ENTER;
    m["ESC"] = KEY_ESC;
    m["SPACE"] = KEY_SPACE;
    m["TAB"] = KEY_TAB;
    m["BACKSPACE"] = KEY_BACKSPACE;
    m["UP"] = KEY_UP;
    m["DOWN"] = KEY_DOWN;
    m["LEFT"] = KEY_LEFT;
    m["RIGHT"] = KEY_RIGHT;
    // letters
    for (char c='A'; c<='Z'; ++c) {
        std::string s(1,c);
        m[s] = KEY_A + (c - 'A');
    }
    // numbers 1..9,0
    m["1"] = KEY_1; m["2"] = KEY_2; m["3"] = KEY_3; m["4"] = KEY_4; m["5"] = KEY_5;
    m["6"] = KEY_6; m["7"] = KEY_7; m["8"] = KEY_8; m["9"] = KEY_9; m["0"] = KEY_0;
    return m;
}

void run_socket_loop(int ufd) {
    // Create unix domain socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); return; }
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path)-1);
    unlink(SOCKET_PATH);
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind socket");
        close(server_fd); server_fd = -1; return;
    }
    if (chmod(SOCKET_PATH, 0660) < 0) {
        // ignore; udev/group approach recommended
    }
    if (listen(server_fd, 5) < 0) { perror("listen"); close(server_fd); server_fd = -1; return; }

    std::cout << "Listening on " << SOCKET_PATH << " for commands\n";
    auto keymap = build_keymap();

    while (true) {
        int client = accept(server_fd, nullptr, nullptr);
        if (client < 0) { perror("accept"); break; }
        char buf[256];
        ssize_t r = read(client, buf, sizeof(buf)-1);
        if (r > 0) {
            buf[r] = 0;
            std::string line(buf);
            // trim newline
            while (!line.empty() && (line.back()=='\n' || line.back()=='\r')) line.pop_back();
            std::cerr << "cmd: " << line << "\n";
            // parse: "<action> <key>"
            std::string action, key;
            size_t pos = line.find(' ');
            if (pos != std::string::npos) {
                action = line.substr(0,pos);
                key = line.substr(pos+1);
            } else {
                action = line;
            }
            for (auto &c : key) c = toupper((unsigned char)c);
            int code = -1;
            if (keymap.count(key)) code = keymap[key];
            else {
                // try numeric keycode
                try {
                    int v = std::stoi(key);
                    code = v;
                } catch (...) { code = -1; }
            }
            if (code < 0) {
                std::string resp = "UNKNOWN_KEY\n";
                write(client, resp.c_str(), resp.size());
                close(client);
                continue;
            }
            bool press = (action == "PRESS" || action == "DOWN");
            bool release = (action == "RELEASE" || action == "UP");
            if (!press && !release) {
                std::string resp = "UNKNOWN_ACTION\n";
                write(client, resp.c_str(), resp.size());
                close(client);
                continue;
            }
            if (send_key(ufd, code, press) == 0) {
                std::string resp = "OK\n";
                write(client, resp.c_str(), resp.size());
            } else {
                std::string resp = "ERR\n";
                write(client, resp.c_str(), resp.size());
            }
        }
        close(client);
    }
}

int main(int argc, char **argv) {
    signal(SIGINT, cleanup_and_exit);
    signal(SIGTERM, cleanup_and_exit);

    if (geteuid() != 0) {
        std::cerr << "Warning: not running as root. Ensure current user has write access to /dev/uinput (udev rule or input group).\n";
    }

    if (!setup_uinput_device()) {
        std::cerr << "Failed to setup uinput device. Exiting.\n";
        return 1;
    }

    run_socket_loop(uinput_fd);

    cleanup_and_exit(0);
    return 0;
}
