#include <QLoggingCategory>

#include "OperateFileTransfer.h"
#include "Plugin.h"

static Q_LOGGING_CATEGORY(log, "Plugin.FileTransfer.Operate")

COperateFileTransfer::COperateFileTransfer(CPlugin *plugin) : COperate(plugin)
{
    qDebug(log) << Q_FUNC_INFO;
}

COperateFileTransfer::~COperateFileTransfer()
{
    qDebug(log) << Q_FUNC_INFO;
}

const qint16 COperateFileTransfer::Version() const
{
    return GetPlugin()->Version().toInt();
}

QWidget *COperateFileTransfer::GetViewer()
{
    return nullptr;
}

int COperateFileTransfer::Start()
{
    int nRet = 0;
    
    return nRet;
}

int COperateFileTransfer::Stop()
{
    int nRet = 0;
    
    return nRet;
}

int COperateFileTransfer::SetGlobalParameters(CParameterPlugin *pPara)
{
    int nRet = 0;
    return nRet;
}

QDialog *COperateFileTransfer::OnOpenDialogSettings(QWidget *parent)
{
    return nullptr;
}
