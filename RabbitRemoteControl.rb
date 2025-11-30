# https://docs.brew.sh/Formula-Cookbook

class Rabbitremotecontrol < Formula
  desc " Rabbit remote control is  is a cross-platform,
  multi-protocol remote control software.
  .
  Allows you to use any device and system in anywhere and remotely manage
  any device and system in any way. 
  .
  It's goal is to be simple, convenient, security and easy to use, improving work efficiency.
  .
  It include remote desktop, remote control, file transfer(FTP, SFTP),
  terminal, remote terminal(SSH, TELNET), player, network tools etc functions.
  .
  Author: Kang Lin <kl222@126.com>
  .
  Donation:
  .
  https://github.com/KangLin/RabbitCommon/raw/master/Src/Resource/image/Contribute.png
  .
  https://gitee.com/kl222/RabbitCommon/raw/master/Src/Resource/image/Contribute.png
  .
  https://gitlab.com/kl222/RabbitCommon/-/raw/master/Src/Resource/image/Contribute.png
  "
  homepage "https://github.com/KangLin/RabbitRemoteControl"
  license "GNU General Public License v3.0"
  # 明确使用 develop 分支
  url "https://github.com/KangLin/RabbitRemoteControl.git",
      using: :git,
      branch: "develop"

  # 使用 `brew install --HEAD RabbitRemoteControl` 来得到 develop 分支
  head "https://github.com/KangLin/RabbitRemoteControl.git", branch: "develop"

  depends_on "cmake" => [:build, :test]
  depends_on "ninja" => :build
  depends_on "pkg-config" => :build
  depends_on "graphviz" => :build
  depends_on "gettext" => :build
  depends_on "doxygen" => :build

  depends_on "openssl"
  depends_on "freerdp"
  depends_on "libvncserver"
  depends_on "libssh"
  depends_on "pcapplusplus"
  depends_on "curl"
  depends_on "qtkeychain"
  depends_on "qtscxml"
  depends_on "qtmultimedia"
  depends_on "qtwebengine"
  depends_on "qtserialport"

  # 如果是GUI应用
  depends_on "qwt" if build.with? "qwt"

  def install
    args = std_cmake_args + %W[
      -DCMAKE_INSTALL_PREFIX=#{prefix}
      -DCMAKE_BUILD_TYPE=Release
    ]
    
    # 创建build目录
    mkdir "build" do
      system "cmake", "..", *args
      system "make", "-j#{ENV.make_jobs}"
      system "make", "install"
    end
    
    # 安装应用程序（如果是macOS .app）
    if OS.mac? && build.stable?
      prefix.install Dir["build/*.app"]
      bin.write_exec_script prefix/"RabbitRemoteControl.app/Contents/MacOS/RabbitRemoteControl"
    end
  end

  def caveats
    <<~EOS
      RabbitRemoteControl has been installed!
      
      To run the application:
        rabbitremotecontrol
      
      For GUI version on macOS, you can find it in:
        #{opt_prefix}/RabbitRemoteControl.app
    EOS
  end

  test do
    # 测试命令行版本
    system "#{bin}/rabbitremotecontrol", "--help"
    
    # 测试版本信息
    assert_match version.to_s, shell_output("#{bin}/rabbitremotecontrol --version")
  end
end
