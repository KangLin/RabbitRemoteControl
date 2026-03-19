<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN">
<context>
    <name>CBackendFtpServer</name>
    <message>
        <location filename="../../BackendFtpServer.cpp" line="60"/>
        <source>Ftp server listen on all address port %1. the lan ip is %2</source>
        <translation>Ftp 服务在所有地址的端口 %1 上监听。局域网地址是 %2</translation>
    </message>
    <message>
        <location filename="../../BackendFtpServer.cpp" line="65"/>
        <source>Failed to Ftp server is listening on %1</source>
        <translation>Ftp 服务在 %1 上监听失败</translation>
    </message>
    <message>
        <location filename="../../BackendFtpServer.cpp" line="74"/>
        <source>Failed: Ftp server is not set to listen on any address</source>
        <translation>失败：Ftp 服务没有设置监听地址</translation>
    </message>
    <message>
        <location filename="../../BackendFtpServer.cpp" line="83"/>
        <source>Failed to Ftp server is listening on %1:%2</source>
        <translation>Ftp 服务在 %1:%2 上监听失败</translation>
    </message>
    <message>
        <location filename="../../BackendFtpServer.cpp" line="94"/>
        <source>Ftp server is listening on </source>
        <translation>Ftp 服务监听在 </translation>
    </message>
</context>
<context>
    <name>CDlgSettings</name>
    <message>
        <location filename="../../DlgSettings.ui" line="14"/>
        <source>Dialog</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../../DlgSettings.ui" line="37"/>
        <source>General</source>
        <translation>普通</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.ui" line="48"/>
        <source>Max connect count:</source>
        <translation>最大连接数：</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.ui" line="75"/>
        <source>Root directory:</source>
        <translation>根目录：</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.ui" line="85"/>
        <source>Browser</source>
        <translation>浏览</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.ui" line="94"/>
        <source>Aollow anonymous login</source>
        <translation>允许匿名登录</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.ui" line="101"/>
        <source>Read-only</source>
        <translation>只读</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.ui" line="108"/>
        <source>Enable listen at all network interface</source>
        <translation>允许在所有网络接口上监听</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.ui" line="123"/>
        <source>Whitelist</source>
        <translation>白名单</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.ui" line="151"/>
        <location filename="../../DlgSettings.ui" line="190"/>
        <location filename="../../DlgSettings.cpp" line="149"/>
        <location filename="../../DlgSettings.cpp" line="163"/>
        <source>Add</source>
        <translation>增加</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.ui" line="158"/>
        <location filename="../../DlgSettings.ui" line="183"/>
        <location filename="../../DlgSettings.cpp" line="151"/>
        <location filename="../../DlgSettings.cpp" line="165"/>
        <source>Delete</source>
        <translation>删除</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.ui" line="168"/>
        <source>Blacklist</source>
        <translation>黑名单</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.cpp" line="23"/>
        <source>Use the following &quot;Enable listen at all network interface&quot;</source>
        <translation>使用下面&quot;允许在所有网络接口上监听&quot;</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.cpp" line="30"/>
        <source>-1: Enable all
 0: Disable all
&gt;0: Connect count</source>
        <translation>-1: 允许所有连接
0: 禁止所有连接
&gt;0: 允许的连接数</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.cpp" line="60"/>
        <source>The IP address and the netmask must be separated by a slash (/).</source>
        <translation>IP 及掩码必须由 (/) 分隔。</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.cpp" line="61"/>
        <source>ag:</source>
        <translation>例如：</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.cpp" line="62"/>
        <source>where n is any value between 0 and 32</source>
        <translation>n 是在 0 到 32 之间的值</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.cpp" line="64"/>
        <source>where n is any value between 0 and 128</source>
        <translation>n 是在 0 到 128 之间的值</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.cpp" line="65"/>
        <source>For IP version 4, accepts as well missing trailing components</source>
        <translation>对于 IP 版本 4，也接受缺少尾部组件的情况</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.cpp" line="66"/>
        <source>(i.e., less than 4 octets, like &quot;192.168.1&quot;), followed or not by a dot. </source>
        <translation>（即少于 4 个八位字节，如“192.168.1”），后面跟着一个点，也可以不跟着一个点。</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.cpp" line="67"/>
        <source>If the netmask is also missing in that case,</source>
        <translation>如果这种情况下网络掩码也缺失，</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.cpp" line="68"/>
        <source>it is set to the number of octets actually passed</source>
        <translation>它被设置为实际传递的八位字节数。</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.cpp" line="69"/>
        <source>(in the example above, it would be 24, for 3 octets).</source>
        <translation>（在上面的例子中，3 个八位字节数的值为 24）。</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.cpp" line="70"/>
        <source>Add IP address and the netmask:</source>
        <translation>增加 IP 及掩码：</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.cpp" line="174"/>
        <source>Add whilte list</source>
        <translation>增加白名单</translation>
    </message>
    <message>
        <location filename="../../DlgSettings.cpp" line="190"/>
        <source>Add black list</source>
        <translation>增加黑名单</translation>
    </message>
</context>
<context>
    <name>CFrmMain</name>
    <message>
        <location filename="../../FrmMain.ui" line="14"/>
        <source>Form</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../../FrmMain.ui" line="35"/>
        <source>Start</source>
        <translation>开始</translation>
    </message>
    <message>
        <location filename="../../FrmMain.ui" line="45"/>
        <source>Settings</source>
        <translation>设置</translation>
    </message>
    <message>
        <location filename="../../FrmMain.ui" line="64"/>
        <source>Connect count:</source>
        <translation>连接数：</translation>
    </message>
    <message>
        <location filename="../../FrmMain.cpp" line="22"/>
        <source>IP</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../../FrmMain.cpp" line="23"/>
        <source>Port</source>
        <translation>端口</translation>
    </message>
    <message>
        <location filename="../../FrmMain.cpp" line="24"/>
        <source>Time</source>
        <translation>时间</translation>
    </message>
    <message>
        <location filename="../../FrmMain.cpp" line="68"/>
        <source>Connect count: Current: %1; Disconnect: %2; Total: %3</source>
        <translation>连接数：当前 %1; 已断开：%2; 总数：%3</translation>
    </message>
    <message>
        <location filename="../../FrmMain.cpp" line="106"/>
        <source>Disconnect</source>
        <translation>断开连接：</translation>
    </message>
</context>
<context>
    <name>COperateFtpServer</name>
    <message>
        <location filename="../../OperateFtpServer.cpp" line="146"/>
        <location filename="../../OperateFtpServer.cpp" line="178"/>
        <source>Start server</source>
        <translation>开始服务器</translation>
    </message>
    <message>
        <location filename="../../OperateFtpServer.cpp" line="190"/>
        <source>Stop server</source>
        <translation>停止服务器</translation>
    </message>
</context>
<context>
    <name>CPluginFtpServer</name>
    <message>
        <location filename="../../PluginFtpServer.cpp" line="41"/>
        <source>FTP(File Transfer Protocol) Server</source>
        <translation>FTP（文件传输协议）服务器</translation>
    </message>
    <message>
        <location filename="../../PluginFtpServer.cpp" line="46"/>
        <source>FTP(File Transfer Protocol) Server: include FTP, FTP-ES.</source>
        <translation>FTP（文件传输协议）服务器：包括 FTP, FTP-ES</translation>
    </message>
    <message>
        <location filename="../../PluginFtpServer.cpp" line="47"/>
        <source>It uses: </source>
        <translation>使用：</translation>
    </message>
    <message>
        <location filename="../../PluginFtpServer.cpp" line="63"/>
        <source>Dependency libraries</source>
        <translation>依赖库</translation>
    </message>
    <message>
        <location filename="../../PluginFtpServer.cpp" line="64"/>
        <source>QFtpServerLib version</source>
        <translation>QFtpServerLib 版本号</translation>
    </message>
</context>
</TS>
