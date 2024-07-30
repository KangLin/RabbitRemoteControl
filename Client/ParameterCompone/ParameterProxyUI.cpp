#include "ParameterProxyUI.h"
#include <QHBoxLayout>
#include <QSpacerItem>

CParameterProxyUI::CParameterProxyUI(QWidget *parent)
    : QWidget(parent),
    m_uiNet(nullptr),
    m_uiSSH(nullptr)
{
    bool bCheck = false;
    setLayout(new QBoxLayout(QBoxLayout::TopToBottom, this));
    QHBoxLayout* pType = new QHBoxLayout(this);
    layout()->addItem(pType);
    
    m_uiNet = new CParameterNetUI(this);
    layout()->addWidget(m_uiNet);
    
    m_uiSSH = new CParameterNetUI(this);
    layout()->addWidget(m_uiSSH);
    
    m_lbType = new QLabel(this);
    m_lbType->setText(tr("Type:"));
    pType->addWidget(m_lbType);
    
    m_cbType = new QComboBox(this);
    pType->addWidget(m_cbType);
    bCheck = connect(m_cbType, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(slotTypeChanged(int)));
    Q_ASSERT(bCheck);
    m_cbType->addItem(tr("None"), (int)CParameterProxy::TYPE::None);
    m_cbType->addItem(tr("Default"), (int)CParameterProxy::TYPE::Default);
    m_cbType->addItem(tr("SockesV5"), (int)CParameterProxy::TYPE::SockesV5);
    m_cbType->addItem(tr("SSH Tunnel"), (int)CParameterProxy::TYPE::SSHTunnel);
    
    layout()->addItem(new QSpacerItem(0, 0,
                                      QSizePolicy::Preferred,
                                      QSizePolicy::Expanding));
}

void CParameterProxyUI::slotTypeChanged(int nIndex)
{
    Q_UNUSED(nIndex);
    int type = m_cbType->currentData().toInt();
    if(m_uiNet)
        m_uiNet->setVisible(type & (int)CParameterProxy::TYPE::SockesV5);
    
    if(m_uiSSH)
        m_uiSSH->setVisible(type & (int)CParameterProxy::TYPE::SSHTunnel);

}

int CParameterProxyUI::SetParameter(CParameterProxy *pParameter)
{
    m_Proxy = pParameter;
    m_uiNet->SetParameter(&m_Proxy->m_SockesV5);
    m_uiSSH->SetParameter(&m_Proxy->m_SSH);
    int index = m_cbType->findData((int)m_Proxy->GetType());
    m_cbType->setCurrentIndex(index);
    return 0;
}

int CParameterProxyUI::slotAccept()
{
    int nRet = 0;
    int type = m_cbType->currentData().toInt();
    switch(type)
    {
    case (int)CParameterProxy::TYPE::SockesV5: {
        nRet = m_uiNet->slotAccept(true);
        if(nRet) return nRet;
        break;
    }
    case (int)CParameterProxy::TYPE::SSHTunnel: {
        nRet = m_uiSSH->slotAccept(true);
        if(nRet) return nRet;
        break;
    }
    default:
        break;
    }

    m_Proxy->SetType((CParameterProxy::TYPE)type);

    return 0;
}