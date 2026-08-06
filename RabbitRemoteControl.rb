# Documentation: https://docs.brew.sh/Formula-Cookbook
#                https://docs.brew.sh/rubydoc/Formula

class Rabbitremotecontrol < Formula
  desc "A open source remote desktop and remote control. Support VNC, RDP, Terminal, SSH, TELNET, network tools, player etc"
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
  depends_on "qtimageformats"
  depends_on "qt5compat"
  depends_on "qtwebsockets"
  depends_on "qtpositioning"
  depends_on "qtwebchannel"
  depends_on "qtpdf"
  depends_on "qtvirtualkeyboard"

  def install
    args = std_cmake_args + %W[
      -DCMAKE_INSTALL_PREFIX=#{prefix}
      -DCMAKE_BUILD_TYPE=Release
      -DCMARK_SHARED=OFF
      -DCMARK_TESTS=OFF
      -DCMARK_STATIC=ON
      -DWITH_CMARK=OFF
      -DWITH_CMARK_GFM=ON
      -DCMAKE_POLICY_VERSION_MINIMUM=3.5
      -DBUILD_FREERDP=ON
    ]

    # 创建build目录
    mkdir "build" do
      system "cmake", "..", *args
      system "cmake", "--build", ".", "--config", "Release", "-j#{ENV.make_jobs}"
      system "cmake", "--install", ".", "--config", "Release", "--strip", "--component", "DependLibraries"
      system "cmake", "--install", ".", "--config", "Release", "--strip", "--component", "Runtime"
      system "cmake", "--install", ".", "--config", "Release", "--strip", "--component", "Plugin"
      system "cmake", "--install", ".", "--config", "Release", "--strip", "--component", "Application"
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
