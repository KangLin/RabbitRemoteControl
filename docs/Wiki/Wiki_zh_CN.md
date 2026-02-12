# Wiki

## 配置数据库

### 配置 MySQL

以下命令都在服务器上执行。

- 检查 MySQL 用户权限

登录MySQL服务器（在服务器上执行。默认密码为空）：

```
$ sudo mysql -uroot -p
```

```
-- 查看现有用户和权限
SELECT user, host FROM mysql.user;

-- 查看特定用户的权限
SHOW GRANTS FOR '用户名'@'192.168.137.1';
```

- 创建或修改用户权限

```
-- 方法1：为特定IP创建用户并授权
CREATE USER '用户名'@'192.168.137.1' IDENTIFIED BY '密码';
GRANT ALL PRIVILEGES ON *.* TO '用户名'@'192.168.137.1';

-- 方法2：允许整个网段
CREATE USER '用户名'@'192.168.137.%' IDENTIFIED BY '密码';
GRANT ALL PRIVILEGES ON *.* TO '用户名'@'192.168.137.%';

-- 方法3：允许所有IP（不推荐生产环境）
CREATE USER '用户名'@'%' IDENTIFIED BY '密码';
GRANT ALL PRIVILEGES ON *.* TO '用户名'@'%';

-- 刷新权限
FLUSH PRIVILEGES;
```

- 修改 MySQL 绑定地址

编辑MySQL配置文件（通常为/etc/mysql/mysql.conf.d/mysqld.cnf或/etc/my.cnf）：

```
[mysqld]
# 允许所有IP连接
bind-address = 0.0.0.0

# 或指定多个IP
# bind-address = 127.0.0.1,192.168.137.1
```

重启MySQL服务：

```
# Ubuntu/Debian
sudo systemctl restart mysql

# CentOS/RHEL
sudo systemctl restart mysqld
```

-  配置防火墙设置

确保防火墙允许MySQL端口（默认3306）：

```
# 查看防火墙状态
sudo ufw status  # Ubuntu
sudo firewall-cmd --list-all  # CentOS

# 开放端口
sudo ufw allow 3306/tcp
# 或
sudo firewall-cmd --permanent --add-port=3306/tcp
sudo firewall-cmd --reload
```

### 在 windows 下使用 ODBC 访问 MySQL 数据库
#### 配置 ODBC

- 下载 MySql  Connector/ODBC 驱动： https://dev.mysql.com/downloads/connector/odbc/
- 下载后安装
- 打开 “控制面板→系统和安全→管理工具→ODBC 数据源(64 位)”。注意：如果你的程序是32位的，就选择　“ODBC　数据源(32　位）”
  - 在“用户 DSN”或“系统 DSN”标签下，添加一个新的数据源，选择 MySQL ODBC 驱动程序，并填写以下信息：
    - 数据源名称（必填）。它与程序设置中的数据库名相同。默认为：remote_control。详见：https://github.com/KangLin/RabbitRemoteControl/blob/48d6d1f3efaed444204cae04322a38e963c66047/Src/Database.cpp#L125
    - 服务器地址
    - 用户名
    - 密码 
    - 数据库名称(可选）。如果填，请先用 [mysql.sql](https://github.com/KangLin/RabbitRemoteControl/blob/master/App/Client/Resource/Database/mysql.sql) 建立好数据库。如果不填，程序会自动建立数据库。

## 问: 当运行 AppImage, 出现下面错误：

```
[root@8bbd00d2d0eb home]# ./RabbitRemoteControl_v0.0.32_Linux_x86_64.AppImage 
qt.qpa.xcb: could not connect to display 
qt.qpa.plugin: From 6.5.0, xcb-cursor0 or libxcb-cursor0 is needed to load the Qt xcb platform plugin.
qt.qpa.plugin: Could not load the Qt platform plugin "xcb" in "" even though it was found.
This application failed to start because no Qt platform plugin could be initialized. Reinstalling the application may fix this problem.

Available platform plugins are: xcb.

Aborted (core dumped)
```

### 答: 这是因为系统没有安装桌面环境。
设置桌面环境。例如：

```
# 安装虚拟桌面 (virtual framebuffer X server for X Version 11)
sudo apt install -y -q xvfb xpra
# 启动桌面
sudo Xvfb :91.0 -ac -screen 0 1200x900x24 &
# 设置桌面环境变量
export DISPLAY=:91.0
```
