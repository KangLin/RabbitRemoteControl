# Termux

## [termux](https://github.com/termux)

### Install the termux applications
- [termux-app](https://github.com/termux/termux-app):
  Termux - a terminal emulator application for Android OS extendible by variety of packages.
- [termux-x11](https://github.com/termux/termux-x11):
  Termux X-server add-on.

### Open termux application
- Update repose

      pkg install root-repo
      pkg install x11-repo

- Update packages

      pkg update
      pkg upgrade

- Install packages

  - Base software

          pkg install vim git curl wget unrar zip unzip

  - Desktop
    - Install desktop(xfce)

          pkg i xfce # from x11-repo

    - [OPTIONS] Running graphical applications [termux-x11](https://github.com/termux/termux-x11#running-graphical-applications)
      - Install termux-x11

            pkg i termux-x11-nightly

      - Open termux-x11 session

            termux-x11 :1 -xstartup "xfce4-session"

    - [OPTIONS] vnc
      - Install tigvervnc

            pkg install tigervnc

      - Start tigervnc service

            vncserver :1

        - The first time you start it, it will show the setup.
          You can set a password and choose whether it's read-only.

              You will require a password to access your desktops.

              Password:
              Verify:

              Would you like to enter a view-only password (y/n)?

      - Stop tigervnc

            vncserver -kill :1

      - Modify ~/.vnc/xstartup

            vim ~/.vnc/xstartup

            # Change the last line to `xfce4-session` & from `twm &`
            xfce4-session &

  - [OPTIONS] Install ssh service
    - Install OpenSSH

          pkg install openssh

    - Check the current username

          whoami

    - Set the current user password

          passwd

    - Manually start the SSH service
    
          sshd

    - Set the service to start automatically
      - Install termux-services：

            pkg install termux-services

      - Exit, then restart termux application. Otherwise, sv-enable will give an error.

            exit

      - Enable sshd service
    
            sv-enable sshd
          
      - Exit, then restart termux application. start automatically ssh service

            exit

    - Check the port: the default port is usually 8022
  
          pkg install iproute2
          ss -tnlp | grep sshd
        
    - Visit
  
          ssh -p 8022 UserName@IP
  - Option software

        pkg i qt-creator qt6

### Compile
- Script
  - build_linux.sh

      build_linux.sh --termux

  - build_termux.sh

- Manual build
  - Set the CMake parameters: `-DCMAKE_SYSTEM_NAME=Linux`

      ```
      # Download source code
      git clone https://github.com/KangLin/RabbitRemoteControl.git
      # Download RabbitComm
      git clone https://github.com/KangLin/RabbitCommon.git
      # Entry project source code
      cd RabbitRemoteControl
      mkdir build
      cmake .. \
          -DCMAKE_SYSTEM_NAME=Linux \
          -DCMAKE_INSTALL_PREFIX=install \
          -DCMARK_SHARED=OFF \
          -DCMARK_TESTS=OFF \
          -DCMARK_STATIC=ON \
          -DWITH_CMARK=OFF \
          -DWITH_CMARK_GFM=ON \
          -DENABLE_UPDATE_TRANSLATIONS=ON \
          -DCMAKE_BUILD_TYPE=Release
      ```
- Build with qtcreator
  - Install qtcreator

    pkg i qtcreator

  - Open the project and build with qtcreator
    - Set the CMake parameters: `-DCMAKE_SYSTEM_NAME=Linux`
