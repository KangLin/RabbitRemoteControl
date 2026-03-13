#include "VirtualKeyboardSettingsDialog.h"
#include <QDebug>
#include <QLocale>
#include <QMessageBox>
#include <QApplication>
#include <QScreen>
#include <QStyleFactory>

CVirtualKeyboardSettingsDialog::CVirtualKeyboardSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_settings(new QSettings("YourCompany", "YourApp", this))
{
    setWindowTitle(tr("虚拟键盘设置"));
    setModal(true);
    setMinimumWidth(400);

    setupUi();
    loadSettings();

    // 连接信号槽
    connect(m_applyButton, &QPushButton::clicked, this, &CVirtualKeyboardSettingsDialog::applySettings);
    connect(m_defaultsButton, &QPushButton::clicked, this, &CVirtualKeyboardSettingsDialog::restoreDefaults);
    connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    connect(m_languageCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CVirtualKeyboardSettingsDialog::onLanguageChanged);
    connect(m_themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CVirtualKeyboardSettingsDialog::onThemeChanged);
}

CVirtualKeyboardSettingsDialog::~CVirtualKeyboardSettingsDialog()
{
}

void CVirtualKeyboardSettingsDialog::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ===== 语言设置组 =====
    QGroupBox *languageGroup = new QGroupBox(tr("输入语言"), this);
    QVBoxLayout *languageLayout = new QVBoxLayout(languageGroup);

    m_languageCombo = new QComboBox(this);
    m_localeList = getAvailableLocales();

    // 填充语言下拉框
    for (const QString &locale : m_localeList) {
        QLocale loc(locale);
        QString displayName = QString("%1 (%2)")
                                  .arg(loc.nativeLanguageName())
                                  .arg(loc.nativeCountryName());
        m_languageCombo->addItem(displayName, locale);
    }

    languageLayout->addWidget(new QLabel(tr("选择输入语言:"), this));
    languageLayout->addWidget(m_languageCombo);
    languageLayout->addStretch();

    // ===== 外观设置组 =====
    QGroupBox *appearanceGroup = new QGroupBox(tr("外观设置"), this);
    QVBoxLayout *appearanceLayout = new QVBoxLayout(appearanceGroup);

    // 主题选择
    QHBoxLayout *themeLayout = new QHBoxLayout();
    themeLayout->addWidget(new QLabel(tr("键盘主题:"), this));
    m_themeCombo = new QComboBox(this);
    QStringList themes = getAvailableStyles();
    themes.prepend("默认"); // 添加默认主题选项
    m_themeCombo->addItems(themes);
    themeLayout->addWidget(m_themeCombo);
    themeLayout->addStretch();
    appearanceLayout->addLayout(themeLayout);

    // 按键大小调节
    QHBoxLayout *keySizeLayout = new QHBoxLayout();
    keySizeLayout->addWidget(new QLabel(tr("按键大小:"), this));
    m_keySizeSlider = new QSlider(Qt::Horizontal, this);
    m_keySizeSlider->setRange(50, 150);
    m_keySizeSlider->setValue(100);
    m_keySizeSlider->setTickPosition(QSlider::TicksBelow);
    m_keySizeSlider->setTickInterval(10);
    keySizeLayout->addWidget(m_keySizeSlider, 1);

    m_keySizeLabel = new QLabel("100%", this);
    keySizeLayout->addWidget(m_keySizeLabel);
    appearanceLayout->addLayout(keySizeLayout);

    // 连接滑块信号更新百分比显示
    connect(m_keySizeSlider, &QSlider::valueChanged, [this](int value) {
        m_keySizeLabel->setText(QString("%1%").arg(value));
    });

    // 辅助选项
    m_smallTextCheck = new QCheckBox(tr("显示按键辅助小文本"), this);
    m_autoCapsCheck = new QCheckBox(tr("自动大写"), this);
    appearanceLayout->addWidget(m_smallTextCheck);
    appearanceLayout->addWidget(m_autoCapsCheck);

    // ===== 反馈设置组 =====
    QGroupBox *feedbackGroup = new QGroupBox(tr("按键反馈"), this);
    QVBoxLayout *feedbackLayout = new QVBoxLayout(feedbackGroup);

    m_keySoundCheck = new QCheckBox(tr("启用按键音"), this);
    m_hapticCheck = new QCheckBox(tr("启用震动反馈"), this);

    feedbackLayout->addWidget(m_keySoundCheck);
    feedbackLayout->addWidget(m_hapticCheck);

    // ===== 按钮区域 =====
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_defaultsButton = new QPushButton(tr("恢复默认"), this);
    m_applyButton = new QPushButton(tr("应用"), this);
    m_okButton = new QPushButton(tr("确定"), this);
    m_cancelButton = new QPushButton(tr("取消"), this);

    buttonLayout->addWidget(m_defaultsButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_applyButton);
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);

    // 将所有组件添加到主布局
    mainLayout->addWidget(languageGroup);
    mainLayout->addWidget(appearanceGroup);
    mainLayout->addWidget(feedbackGroup);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
}

void CVirtualKeyboardSettingsDialog::loadSettings()
{
    // 加载语言设置
    QString savedLocale = m_settings->value("virtualkeyboard/locale", "en_US").toString();
    int localeIndex = m_localeList.indexOf(savedLocale);
    if (localeIndex >= 0) {
        m_languageCombo->setCurrentIndex(localeIndex);
    }

    // 加载主题设置
    QString savedTheme = m_settings->value("virtualkeyboard/theme", "默认").toString();
    int themeIndex = m_themeCombo->findText(savedTheme);
    if (themeIndex >= 0) {
        m_themeCombo->setCurrentIndex(themeIndex);
    }

    // 加载其他设置
    m_keySoundCheck->setChecked(m_settings->value("virtualkeyboard/keySound", true).toBool());
    m_hapticCheck->setChecked(m_settings->value("virtualkeyboard/haptic", false).toBool());
    m_keySizeSlider->setValue(m_settings->value("virtualkeyboard/keySize", 100).toInt());
    m_smallTextCheck->setChecked(m_settings->value("virtualkeyboard/smallText", true).toBool());
    m_autoCapsCheck->setChecked(m_settings->value("virtualkeyboard/autoCaps", true).toBool());
}

void CVirtualKeyboardSettingsDialog::saveSettings()
{
    m_settings->setValue("virtualkeyboard/locale",
                         m_languageCombo->currentData().toString());
    m_settings->setValue("virtualkeyboard/theme", m_themeCombo->currentText());
    m_settings->setValue("virtualkeyboard/keySound", m_keySoundCheck->isChecked());
    m_settings->setValue("virtualkeyboard/haptic", m_hapticCheck->isChecked());
    m_settings->setValue("virtualkeyboard/keySize", m_keySizeSlider->value());
    m_settings->setValue("virtualkeyboard/smallText", m_smallTextCheck->isChecked());
    m_settings->setValue("virtualkeyboard/autoCaps", m_autoCapsCheck->isChecked());

    m_settings->sync();
}

void CVirtualKeyboardSettingsDialog::applySettings()
{
    // 应用语言设置
    QString selectedLocale = m_languageCombo->currentData().toString();
    applyLanguage(selectedLocale);

    // 应用主题设置
    applyTheme(m_themeCombo->currentText());

    // 应用其他设置
    applyKeySound(m_keySoundCheck->isChecked());
    applyHapticFeedback(m_hapticCheck->isChecked());
    applyKeySize(m_keySizeSlider->value());

    // 辅助选项
    // QVirtualKeyboardSettings *vkSettings = QVirtualKeyboardSettings::instance();
    // if (vkSettings) {
    //     vkSettings->setSmallTextVisible(m_smallTextCheck->isChecked());
    //     // 自动大写可能需要通过其他方式设置
    // }

    // 保存到QSettings
    saveSettings();

    qDebug() << "Virtual keyboard settings applied";
}

void CVirtualKeyboardSettingsDialog::restoreDefaults()
{
    m_languageCombo->setCurrentIndex(m_localeList.indexOf("en_US"));
    m_themeCombo->setCurrentIndex(0); // "默认"
    m_keySoundCheck->setChecked(true);
    m_hapticCheck->setChecked(false);
    m_keySizeSlider->setValue(100);
    m_smallTextCheck->setChecked(true);
    m_autoCapsCheck->setChecked(true);
}

void CVirtualKeyboardSettingsDialog::onLanguageChanged(int index)
{
    if (index >= 0 && index < m_localeList.size()) {
        // 可以在这里实时预览语言切换效果
        QString locale = m_localeList[index];
        qDebug() << "Language preview:" << locale;
    }
}

void CVirtualKeyboardSettingsDialog::onThemeChanged(int index)
{
    Q_UNUSED(index);
    // 主题预览逻辑
}

QStringList CVirtualKeyboardSettingsDialog::getAvailableLocales()
{
    // 返回支持的locale列表
    // 这取决于你的Qt构建包含的语言包
    return QStringList()
           << "en_US"   // 英语
           << "zh_CN"   // 简体中文
           << "zh_TW"   // 繁体中文
           << "ja_JP"   // 日语
           << "ko_KR"   // 韩语
           << "fr_FR"   // 法语
           << "de_DE"   // 德语
           << "ru_RU"   // 俄语
           << "es_ES"   // 西班牙语
           << "ar_SA"   // 阿拉伯语
           << "hi_IN";  // 印地语
}

QStringList CVirtualKeyboardSettingsDialog::getAvailableStyles()
{
    // 返回可用的键盘样式
    return QStyleFactory::keys();
}

void CVirtualKeyboardSettingsDialog::applyLanguage(const QString &locale)
{
    // QVirtualKeyboardSettings *settings = QVirtualKeyboardSettings::instance();
    // if (settings) {
    //     settings->setLocale(locale);
    // }
}

void CVirtualKeyboardSettingsDialog::applyTheme(const QString &theme)
{
    if (theme != "默认") {
        // 应用Qt样式表
        qApp->setStyle(theme);
    } else {
        // 恢复默认样式
        qApp->setStyle(QStyleFactory::create("Fusion"));
    }
}

void CVirtualKeyboardSettingsDialog::applyKeySound(bool enabled)
{
    // 按键音的实现通常需要与平台相关
    // 这里可以通过QSettings保存设置，在键盘事件中检查
    qDebug() << "Key sound:" << enabled;

    // 实际使用时，你需要在按键处理代码中检查这个设置
}

void CVirtualKeyboardSettingsDialog::applyHapticFeedback(bool enabled)
{
    // 震动反馈的实现通常需要平台特定API
    qDebug() << "Haptic feedback:" << enabled;
}

void CVirtualKeyboardSettingsDialog::applyKeySize(int size)
{
    // 按键大小的实现通常需要通过QML或样式表调整
    // 这里保存设置，然后在键盘初始化时使用
    qDebug() << "Key size:" << size << "%";

    // 可以设置一个环境变量或全局属性，供QML使用
    qputenv("QT_VIRTUALKEYBOARD_KEY_SIZE", QByteArray::number(size));
}
