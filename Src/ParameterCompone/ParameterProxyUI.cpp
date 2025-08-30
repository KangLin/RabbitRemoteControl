// Author: Kang Lin <kl222@126.com>
#include "ParameterProxyUI.h"
#include <QHBoxLayout>
#include <QSpacerItem>

CParameterProxyUI::CParameterProxyUI(QWidget *parent)
    : CParameterUI(parent)
    , m_uiHttp(nullptr)
    , m_uiSocksV5(nullptr)
    , m_uiSSH(nullptr)
{
    bool bCheck = false;
    setLayout(new QBoxLayout(QBoxLayout::TopToBottom, this));
    Q_ASSERT(layout());

    QHBoxLayout* pType = new QHBoxLayout(this);
    layout()->addItem(pType);

    m_uiHttp = new CParameterNetUI(this);
    layout()->addWidget(m_uiHttp);

    m_uiSocksV5 = new CParameterNetUI(this);
    layout()->addWidget(m_uiSocksV5);

    m_uiSSH = new CParameterSSHTunnelUI(this);
    layout()->addWidget(m_uiSSH);

    m_lbType = new QLabel(this);
    m_lbType->setText(tr("Type:"));
    pType->addWidget(m_lbType);

    m_cbType = new QComboBox(this);
    pType->addWidget(m_cbType);
    bCheck = connect(m_cbType, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(slotTypeChanged(int)));
    Q_ASSERT(bCheck);

    layout()->addItem(new QSpacerItem(0, 0,
                                      QSizePolicy::Preferred,
                                      QSizePolicy::Expanding));
}

void CParameterProxyUI::slotTypeChanged(int nIndex)
{
    Q_UNUSED(nIndex);
    int type = m_cbType->currentData().toInt();
    if(m_uiHttp)
        m_uiHttp->setVisible((int)CParameterProxy::TYPE::Http == type);
    if(m_uiSocksV5)
        m_uiSocksV5->setVisible((int)CParameterProxy::TYPE::SocksV5 == type);
    if(m_uiSSH)
        m_uiSSH->setVisible((int)CParameterProxy::TYPE::SSHTunnel == type);
}

int CParameterProxyUI::SetParameter(CParameter *pParameter)
{
    m_Proxy = qobject_cast<CParameterProxy*>(pParameter);
    if(!m_Proxy)
        return -1;
    m_uiHttp->SetParameter(&m_Proxy->m_Http);
    m_uiSocksV5->SetParameter(&m_Proxy->m_SocksV5);
    m_uiSSH->SetParameter(&m_Proxy->m_SSH);
    
    m_cbType->clear();
    foreach(auto t, m_Proxy->GetType())
    {
        m_cbType->addItem(m_Proxy->ConvertTypeToName(t), (int)t);
    }
    
    int index = m_cbType->findData((int)m_Proxy->GetUsedType());
    m_cbType->setCurrentIndex(index);
    return 0;
}

bool CParameterProxyUI::CheckValidity(bool validity)
{
    bool bRet = true;
    int type = m_cbType->currentData().toInt();
    switch(type)
    {
    case (int)CParameterProxy::TYPE::SocksV5: {
        bRet = m_uiSocksV5->CheckValidity(validity);
        if(!bRet) return bRet;
        break;
    }
    case (int)CParameterProxy::TYPE::Http: {
        bRet = m_uiHttp->CheckValidity(validity);
        if(!bRet) return bRet;
        break;
    }
    case (int)CParameterProxy::TYPE::SSHTunnel: {
        bRet = m_uiSSH->CheckValidity(validity);
        if(!bRet) return bRet;
        break;
    }
    default:
        break;
    }

    return bRet;
}

int CParameterProxyUI::Accept()
{
    int nRet = 0;
    int type = m_cbType->currentData().toInt();
    switch(type)
    {
    case (int)CParameterProxy::TYPE::SocksV5: {
        nRet = m_uiSocksV5->Accept();
        if(nRet) return nRet;
        break;
    }
    case (int)CParameterProxy::TYPE::Http: {
        nRet = m_uiHttp->Accept();
        if(nRet) return nRet;
        break;
    }
    case (int)CParameterProxy::TYPE::SSHTunnel: {
        nRet = m_uiSSH->Accept();
        if(nRet) return nRet;
        break;
    }
    default:
        break;
    }

    m_Proxy->SetUsedType((CParameterProxy::TYPE)type);

    return 0;
}
