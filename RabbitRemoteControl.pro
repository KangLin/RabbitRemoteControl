TEMPLATE = subdirs
DESTDIR = $$OUT_PWD/bin
isEmpty(PREFIX) {
    qnx: PREFIX = /tmp/$${TARGET}
    else: android: PREFIX = /.
    else: unix: PREFIX = /opt/$${TARGET}
    else: PREFIX = $$OUT_PWD/../install
}

isEmpty(RabbitCommon_DIR): RabbitCommon_DIR=$$(RabbitCommon_DIR)
isEmpty(RabbitCommon_DIR): RabbitCommon_DIR=$$PWD/../RabbitCommon
!isEmpty(RabbitCommon_DIR): exists("$${RabbitCommon_DIR}/Src/RabbitCommon.pri") {
    RabbitCommon.file = 3th_libs/RabbitCommon.pro
} else {
    message("1. Please download RabbitCommon source code from https://github.com/KangLin/RabbitCommon ag:")
    message("   git clone https://github.com/KangLin/RabbitCommon.git")
    error  ("2. Then set value RabbitCommon_DIR to download root dirctory")
}

App.depends = RabbitCommon
CONFIG *= ordered
SUBDIRS = RabbitCommon App

OTHER_FILES += CMakeLists.txt \
    *.md \
    Install/* \
    *.yml

other.files = License.md Authors.md ChangeLog.md Authors_zh_CN.md \
              ChangeLog_zh_CN.md
android: other.path = $$PREFIX/assets
else: other.path = $$PREFIX
other.CONFIG += no_check_exist
INSTALLS += other
