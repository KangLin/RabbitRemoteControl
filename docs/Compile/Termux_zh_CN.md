# Termux

## [termux](https://github.com/termux)

### 安装 termux 应用

- [termux-app](https://github.com/termux/termux-app):
  Termux -　android 系统的终端模拟器应用

- [termux-x11](https://github.com/termux/termux-x11):
  Termux X-server add-on.

### 打开 termux 应用
- 更新仓库

      pkg install root-repo
      pkg install x11-repo

- 更新软件包

      pkg update
      pkg upgrade

- 安装软件包

  - [可选] 安装 ssh 服务
    - 安装 OpenSSH

          pkg install openssh

    - 查询当前用户名

          whoami

    - 设置当前用户登录密码

          passwd

    - 手动启动 ssh 服务

          sshd

    - 设置服务自启动
      - 安装 termux-services：

            pkg install termux-services

      - 退出终端，并重启 termux 应用。否则　sv-enable 会出错。

            exit

      - 允许 sshd 服务自启动
    
            sv-enable sshd
          
      - 退出终端，并重应用 termux 应用。才能自动开启 sshd

            exit

    - 查看端口：默认端口通常是 8022。通常没有权限
  
          pkg install iproute2
          ss -tnlp | grep sshd
        
    - 访问。默认端口通常是 8022
  
          ssh -p 8022 用户名@手机IP

  - 基本软件
  
          pkg install git curl wget unrar zip unzip
  
  - 桌面
    - 安装桌面环境(xfce)
  
          pkg i xfce # 在 x11-repo　库中

    - [可选] 运行图形应用[termux-x11](https://github.com/termux/termux-x11#running-graphical-applications)
      - 安装 termux-x11
    
            pkg i termux-x11-nightly

      - 打开 termux-x11 会话
    
            termux-x11 :1 -xstartup "xfce4-session"

    - [可选] 安装 tigervnc
      - 安装　tigervnc 服务

            pkg install tigervnc

      - 启动 tigervnc 服务
      
            vncserver :1 # 端口为　5901

        - 第一次启动会显示配置。设置密码和是否只读

              You will require a password to access your desktops.
            
              Password:
              Verify:
            
              Would you like to enter a view-only password (y/n)?
        
      - 停止 tigervnc 服务

            vncserver -kill :1

      - 修改启动文件 ~/.vnc/xstartup

            vim ~/.vnc/xstartup
            
            #将最后一行由
            twm &
            #改成
            xfce4-session &

      - 再次启动 tigervnc 服务

            vncserver :1 # 端口为　5901

      - 可以使用 vnc 客户端（例如：本项目）进行访问

### 编译

- 脚本：
  - build_linux.sh
  - build_termux.sh
