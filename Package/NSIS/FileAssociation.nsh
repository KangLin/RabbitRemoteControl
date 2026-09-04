; FileAssociation.nsh
; 通用的文件关联和协议注册函数

!ifndef FILEASSOCIATION_NSH
!define FILEASSOCIATION_NSH

!include "LogicLib.nsh"

;----------------------------------------------
; 文件关联注册函数
; 使用说明：
; !insertmacro RegisterFileAssociation "rrc" "MyApp.RRCFile.1" \
;   "My App Session File" "$INSTDIR\app.ico" "$INSTDIR\bin\MyApp.exe"
;
;HKEY_CURRENT_USER\Software\Classes
;├── .rrc                          // 1. 文件扩展名
;│   └── (Default) = "RabbitRemoteControl.RRCFile.1"  // 指向刚才定义的ProgID
;├── RabbitRemote.RRCFile.1        // 2. ProgID主键
;│   ├── (Default) = "Rabbit Remote Control Session"  // 文件类型描述
;│   └── shell
;│       └── open
;│           └── command
;│               └── (Default) = "\"C:\Program Files\RabbitRemote\YourApp.exe\" \"%1\""
;(Default) 值：在 .rrc 键下，将默认值设为你的 ProgID RabbitRemote.RRCFile.1，这就建立了扩展名到程序的关联。
;command 键：这里是关键，它的默认值指定了双击文件时运行什么程序。"%1" 会被替换为被双击文件的完整路径，一定要用引号括起来，防止路径包含空格时出错。
!macro RegisterFileAssociation EXTENSION PROGID DESCRIPTION PRODUCT_NAME ICON_PATH EXE_PATH
  ; 使用 HKCU 或 HKLM
  ; 1. 注册文件扩展名 .rrc
  WriteRegStr HKCU "Software\Classes\.${EXTENSION}" "" "${PROGID}"
  WriteRegStr HKCU "Software\Classes\.${EXTENSION}" "Content Type" "application/${EXTENSION}"
  ; 2. 创建 ProgID
  WriteRegStr HKCU "Software\Classes\${PROGID}" "" "${DESCRIPTION}"
  WriteRegStr HKCU "Software\Classes\${PROGID}" "AppUserModelID" "${PRODUCT_NAME}"
  WriteRegStr HKCU "Software\Classes\${PROGID}\DefaultIcon" "" "${ICON_PATH},0"
  ; 3. 关联打开命令
  WriteRegStr HKCU "Software\Classes\${PROGID}\shell\open\command" "" `"${EXE_PATH}" "%1"`
  ; 4. 关联 MIME 类型
  WriteRegStr HKCU "Software\Classes\MIME\Database\Content Type\application/${EXTENSION}" "Extension" ".${EXTENSION}"
  ; 5. 注册为候选应用程序（可选）
  WriteRegStr HKCU "Software\Classes\Applications\${PRODUCT_NAME}\SupportedTypes" ".${EXTENSION}" ""
  WriteRegStr HKCU "Software\Classes\Applications\${PRODUCT_NAME}\shell\open\command" "" `"${APP_EXE}" "%1"`
  ; 6. 设置友好名称
  WriteRegStr HKCU "Software\Classes\Applications\${PROGID}" "FriendlyAppName" "${DESCRIPTION}"

  ; 通知系统更新
  System::Call 'Shell32::SHChangeNotify(i 0x08000000, i 0, i 0, i 0)'
!macroend

!macro UnregisterFileAssociation EXTENSION PROGID
  DeleteRegKey HKCU "Software\Classes\${EXTENSION}"
  DeleteRegKey HKCU "Software\Classes\${PROGID}"
  DeleteRegKey HKCU "Software\Classes\MIME\Database\Content Type\application/${EXTENSION}"
  DeleteRegKey HKCU "Software\Classes\Applications\${PRODUCT_NAME}"

  ; 通知系统更新
  System::Call 'Shell32::SHChangeNotify(i 0x08000000, i 0, i 0, i 0)'
!macroend

;----------------------------------------------
; URL 协议注册函数
; 使用说明：
; !insertmacro RegisterURLProtocol "rrc" "$INSTDIR\app.ico" "$INSTDIR\bin\MyApp.exe"
;
;HKEY_CURRENT_USER\Software\Classes
;└── rrc                           // 1. 自定义协议名称
;    ├── (Default) = "URL:rrc Protocol"  // 协议描述
;    ├── URL Protocol = ""               // 2. 关键标记！内容为空
;    └── shell
;        └── open
;            └── command
;                └── (Default) = "\"C:\Program Files\RabbitRemote\YourApp.exe\" \"%1\""
;URL Protocol：这个空字符串值是必须的，它是 Windows 识别这是一个自定义协议处理程序的标志。
;%1： 当用户点击形如 rrc://open/session 的链接时，整个链接字符串会被作为参数传递给 YourApp.exe，你的程序需要自行解析这个参数来执行相应操作。
!macro RegisterURLProtocol PROTOCOL ICON_PATH EXE_PATH
  ; 1. 注册协议
  WriteRegStr HKCU "Software\Classes\${PROTOCOL}" "" "URL:${PROTOCOL} Protocol"
  WriteRegStr HKCU "Software\Classes\${PROTOCOL}" "URL Protocol" ""

  ; 2. 设置协议图标
  WriteRegStr HKCU "Software\Classes\${PROTOCOL}\DefaultIcon" "" "${ICON_PATH},0"

  ; 3. 设置打开命令
  WriteRegStr HKCU "Software\Classes\${PROTOCOL}\shell\open\command" "" `"${EXE_PATH}" "%1"`

  ; 4. 设置友好名称（可选）
  WriteRegStr HKCU "Software\Classes\${PROTOCOL}" "FriendlyName" "${PRODUCT_NAME} Protocol"

  ; 5. 设置应用用户模型ID（可选）
  WriteRegStr HKCU "Software\Classes\${PROTOCOL}" "AppUserModelID" "${PRODUCT_NAME}"

  ; 通知系统更新
  System::Call 'Shell32::SHChangeNotify(i 0x08000000, i 0, i 0, i 0)'
!macroend

!macro UnregisterURLProtocol PROTOCOL
  DeleteRegKey HKCU "Software\Classes\${PROTOCOL}"

  ; 通知系统更新
  System::Call 'Shell32::SHChangeNotify(i 0x08000000, i 0, i 0, i 0)'
!macroend

!endif ; FILEASSOCIATION_NSH