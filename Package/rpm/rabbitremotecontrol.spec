# See: https://rpm-software-management.github.io/rpm/manual/spec.html

Name:           rabbitremotecontrol
Version:        0.0.32
Release:        1%{?dist}
Summary:        Rabbit remote control is is a cross-platform, multi-protocol remote control software.

License:        GPL-3.0
URL:            https://github.com/KangLin/RabbitRemoteControl
Source0:        RabbitRemoteControl.tar.gz

Vendor:         Kang Lin <kl222@126.com>

BuildRequires: make git rpm-build rpmdevtools 
BuildRequires: gcc-c++
BuildRequires: automake autoconf libtool gettext gettext-autopoint
BuildRequires: cmake desktop-file-utils appstream
BuildRequires: curl wget
BuildRequires: libssh-devel libicu-devel lzo-devel libgcrypt-devel

# Qt6
BuildRequires: qt6-qttools-devel qt6-qtbase-devel
BuildRequires: qt6-qt5compat-devel qt6-qtmultimedia-devel qt6-qtscxml-devel
BuildRequires: qt6-qtserialport-devel qt6-qtsvg-devel

BuildRequires: openssl-devel libpng-devel libjpeg-turbo-devel pixman-devel ffmpeg-free-devel
BuildRequires: libvncserver-devel cmark-devel libpcap-devel
BuildRequires: libwinpr-devel freerdp-devel freerdp fuse3-devel
# TigerVNC
BuildRequires: libpciaccess-devel freetype-devel pam-devel
BuildRequires: gnutls-devel nettle-devel gmp-devel
BuildRequires: zlib-devel systemd-devel
# X11/graphics dependencies
BuildRequires: libXext-devel libX11-devel libXi-devel libXfixes-devel libXtst-devel
BuildRequires: libXdamage-devel libXrandr-devel libXt-devel libXdmcp-devel
BuildRequires: libXinerama-devel mesa-libGL-devel libxshmfence-devel
BuildRequires: libdrm-devel mesa-libgbm-devel
BuildRequires: libxkbfile-devel libXfont2-devel

Requires: openssl libpng libjpeg-turbo pixman ffmpeg-free libssh libicu lzo libgcrypt
Requires: libvncserver libwinpr freerdp fuse3 cmark libpcap
Requires: zlib gnutls nettle gmp pam 
Requires: qt6-qtbase qt6-qtmultimedia qt6-qt5compat qt6-qtmultimedia qt6-qtscxml
Requires: qt6-qtserialport qt6-qtsvg

Requires: libXext libX11 libXi libXfixes libXtst
Requires: libXdamage libXrandr libXt libXdmcp
Requires: libXinerama mesa-libGL libxshmfence
Requires: libdrm mesa-libgbm
Requires: libxkbfile libXfont2

%description
Rabbit remote control is  is a cross-platform,
multi-protocol remote control software.

Allows you to use any device and system in anywhere and remotely manage
any device and system in any way. 

It include remote desktop, remote terminal etc remote control functions.

Author: Kang Lin <kl222@126.com>

Donation:

https://github.com/KangLin/RabbitCommon/raw/master/Src/Resource/image/Contribute.png

https://gitee.com/kl222/RabbitCommon/raw/master/Src/Resource/image/Contribute.png

https://gitlab.com/kl222/RabbitCommon/-/raw/master/Src/Resource/image/Contribute.png

# 预备参数
%prep
%setup -q -n RabbitRemoteControl

%generate_buildrequires

%build
echo "-- RPM_BUILD_DIR: $RPM_BUILD_DIR"
echo "-- RPM_BUILD_ROOT: $RPM_BUILD_ROOT"
echo "-- RPM_SOURCE_DIR: $RPM_SOURCE_DIR"
echo "-- RPM_SPECPARTS_DIR: $RPM_SPECPARTS_DIR"
echo "-- pwd: `pwd`"

cmake . -B $RPM_BUILD_DIR  \
    -DCMAKE_INSTALL_PREFIX=/opt/RabbitRemoteControl -DCMAKE_BUILD_TYPE=Release \
    -DRabbitCommon_ROOT=${RabbitCommon_ROOT}
cmake --build $RPM_BUILD_DIR --config Release --parallel $(nproc)

%install
cmake --install $RPM_BUILD_DIR --config Release \
    --component DependLibraries \
    --prefix ${RPM_BUILD_ROOT}/opt/RabbitRemoteControl
cmake --install $RPM_BUILD_DIR --config Release \
    --component Runtime \
    --prefix ${RPM_BUILD_ROOT}/opt/RabbitRemoteControl
cmake --install $RPM_BUILD_DIR --config Release \
    --component Application \
    --prefix ${RPM_BUILD_ROOT}/opt/RabbitRemoteControl
cmake --install $RPM_BUILD_DIR --config Release \
    --component Plugin \
    --prefix ${RPM_BUILD_ROOT}/opt/RabbitRemoteControl

# 安装前需要做的任务，如：创建用户
%pre

# 安装后需要做的任务 如：自动启动的任务
%post
INSTALL_ROOT=/opt/RabbitRemoteControl
if [ ! -f /usr/share/applications/io.github.KangLin.RabbitRemoteControl.desktop ]; then
    if [ -f $INSTALL_ROOT/share/applications/io.github.KangLin.RabbitRemoteControl.desktop ]; then
        ln -s $INSTALL_ROOT/share/applications/io.github.KangLin.RabbitRemoteControl.desktop /usr/share/applications/io.github.KangLin.RabbitRemoteControl.desktop
    fi
fi
if [ ! -f /usr/share/pixmaps/io.github.KangLin.RabbitRemoteControl.svg ]; then
    if [ -f $INSTALL_ROOT/share/icons/hicolor/scalable/apps/io.github.KangLin.RabbitRemoteControl.svg ]; then
        if [ ! -d /usr/share/pixmaps ]; then
            mkdir -p /usr/share/pixmaps
        fi
        ln -s $INSTALL_ROOT/share/icons/hicolor/scalable/apps/io.github.KangLin.RabbitRemoteControl.svg /usr/share/pixmaps/io.github.KangLin.RabbitRemoteControl.svg
    fi
fi
if [ ! -f /usr/bin/rabbitremotecontrol ]; then
    ln -s $INSTALL_ROOT/bin/RabbitRemoteControlApp /usr/bin/rabbitremotecontrol
fi

if [ -d $INSTALL_ROOT/etc ]; then
    chmod -R a+rw $INSTALL_ROOT/etc
fi

# 卸载前需要做的任务 如：停止任务
%preun
INSTALL_ROOT=/opt/RabbitRemoteControl
rm -fr /usr/share/applications/io.github.KangLin.RabbitRemoteControl.desktop
rm -fr /usr/share/pixmaps/io.github.KangLin.RabbitRemoteControl.svg
rm -fr /usr/bin/rabbitremotecontrol

# 卸载后需要做的任务 如：删除用户，删除/备份业务数据
%postun

# 清除上次编译生成的临时文件
%clean

# 设置文件属性，包含编译文件需要生成的目录、文件以及分配所对应的权限
%files
/opt/RabbitRemoteControl/*

# 修改历史
%changelog
* Fri Jan 01 2025 Kang Lin <kl222@126.com> - 0.0.32-1
- Initial RPM package
