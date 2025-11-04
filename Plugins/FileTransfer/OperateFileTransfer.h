// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#pragma once
#include "Operate.h"
#include "BackendThread.h"
#include "FrmFileTransfer.h"
#include "ParameterFileTransfer.h"

/*!
 * \brief File transfer operate interface
 */
class COperateFileTransfer : public COperate
{
    Q_OBJECT
public:
    explicit COperateFileTransfer(CPlugin *plugin);
    virtual ~COperateFileTransfer();

    /*!
     * \~chinese
     * 新建后端实例。它的所有者是调用者，
     * 如果调用者不再使用它，调用者必须负责释放它。
     *
     * \~english New CBackend. the ownership is caller.
     *        if don't use, the caller must delete it.
     * \~see CBackendThread
     */
    [[nodiscard]] Q_INVOKABLE virtual CBackend* InstanceBackend();

    virtual const QString Id() override;
    virtual const QString Name() override;
    virtual const QString Description() override;
    virtual const QString Protocol() const override;
    virtual const qint16 Version() const override;
    virtual QWidget *GetViewer() override;
    virtual int Start() override;
    virtual int Stop() override;

    CParameterFileTransfer* GetParameter();

protected:
    virtual int SetGlobalParameters(CParameterPlugin *pPara) override;

private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;
    
private Q_SLOTS:
    void slotCopyUrlToClipboard(const QString& szPath);

private:
    CFrmFileTransfer* m_frmFileTransfer;
    CParameterFileTransfer m_Parameter;
    CBackendThread* m_pThread;

protected:
    virtual int Load(QSettings &set) override;
    virtual int Save(QSettings &set) override;

    virtual int Initial() override;
    virtual int Clean() override;
};
