// Author: Kang Lin <kl222@126.com>

#include "PluginViewer.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"
#include <QLocale>
#include <QDebug>
#include <QApplication>
#include <QDir>

CPluginViewer::CPluginViewer(QObject *parent) : QObject(parent)
{
}

CPluginViewer::~CPluginViewer()
{
    qApp->removeTranslator(&m_Translator);
}

int CPluginViewer::InitTranslator()
{
    QString szTranslatorFile = RabbitCommon::CDir::Instance()->GetDirPluginsTranslation("plugins/Viewer")
            + QDir::separator() + Name() + "_" + QLocale::system().name() + ".qm";
    if(!m_Translator.load(szTranslatorFile))
    {
        LOG_MODEL_ERROR("CPluginViewer", "Open translator file fail:",
                        szTranslatorFile.toStdString().c_str());
        return -1;
    }
    qApp->installTranslator(&m_Translator);
    return 0;
}

const QString CPluginViewer::Id() const
{
    return Protol() + ":" + Name();
}

const QIcon CPluginViewer::Icon() const
{
    return QIcon(":/image/Connect");
}
