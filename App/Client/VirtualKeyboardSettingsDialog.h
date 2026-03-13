
#pragma once

#include <QDialog>
#include <QSettings>
#include <QComboBox>
#include <QCheckBox>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDialogButtonBox>

#include <QtVirtualKeyboard>

class CVirtualKeyboardSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CVirtualKeyboardSettingsDialog(QWidget *parent = nullptr);
    ~CVirtualKeyboardSettingsDialog();

    // 加载保存的设置
    void loadSettings();
    // 保存当前设置
    void saveSettings();

private slots:
    // 应用设置（不关闭对话框）
    void applySettings();
    // 恢复默认设置
    void restoreDefaults();
    // 语言选择变化时的处理
    void onLanguageChanged(int index);
    // 主题选择变化时的处理
    void onThemeChanged(int index);

private:
    // 初始化UI
    void setupUi();
    // 获取可用的语言列表
    QStringList getAvailableLocales();
    // 获取可用的键盘主题
    QStringList getAvailableStyles();
    // 应用语言设置
    void applyLanguage(const QString &locale);
    // 应用主题设置
    void applyTheme(const QString &theme);
    // 应用按键音设置
    void applyKeySound(bool enabled);
    // 应用按键震动设置
    void applyHapticFeedback(bool enabled);
    // 应用按键大小设置
    void applyKeySize(int size);

private:
    // UI组件
    QComboBox *m_languageCombo;
    QComboBox *m_themeCombo;
    QCheckBox *m_keySoundCheck;
    QCheckBox *m_hapticCheck;
    QSlider *m_keySizeSlider;
    QLabel *m_keySizeLabel;
    QCheckBox *m_smallTextCheck;
    QCheckBox *m_autoCapsCheck;
    QPushButton *m_applyButton;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
    QPushButton *m_defaultsButton;

    // 设置存储
    QSettings *m_settings;

    // 语言列表和当前索引
    QStringList m_localeList;
    int m_currentLocaleIndex;
};
