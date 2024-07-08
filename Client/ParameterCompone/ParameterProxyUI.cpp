#include "ParameterProxyUI.h"
#include <QHBoxLayout>
#include <QSpacerItem>

CParameterProxyUI::CParameterProxyUI(QWidget *parent)
    : QWidget{parent}
{
    bool bCheck = false;
    setLayout(new QBoxLayout(QBoxLayout::TopToBottom, this));
    QHBoxLayout* pType = new QHBoxLayout(this);
    layout()->addItem(pType);
    
    m_lbType = new QLabel(this);
    m_lbType->setText(tr("Type:"));
    pType->addWidget(m_lbType);
    
    m_cbType = new QComboBox(this);
    m_cbType->addItem(tr("No"), (int)CParameterProxy::TYPE::No);
    m_cbType->addItem(tr("Socket5"), (int)CParameterProxy::TYPE::Socket5);
    pType->addWidget(m_cbType);
    bCheck = connect(m_cbType, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(slotTypeChanged(int)));
    Q_ASSERT(bCheck);

    m_uiSocket5 = new CParameterNetUI(this);
    layout()->addWidget(m_uiSocket5);
    
    layout()->addItem(new QSpacerItem(0, 0,
                                      QSizePolicy::Preferred,
                                      QSizePolicy::Expanding));
}

void CParameterProxyUI::slotTypeChanged(int nIndex)
{
    switch(m_cbType->currentData().toInt())
    {
    case (int)CParameterProxy::TYPE::No: {
        m_uiSocket5->setVisible(false);
        break;
    }
    case (int)CParameterProxy::TYPE::Socket5: {
        m_uiSocket5->setVisible(true);
        break;
    }
    }
}

int CParameterProxyUI::SetParameter(CParameterProxy *pParameter)
{
    m_Proxy = pParameter;
    
    m_uiSocket5->SetParameter(&m_Proxy->m_Socket5);
    
    return 0;
}

int CParameterProxyUI::slotAccept()
{
    int nRet = 0;
    switch(m_cbType->currentData().toInt())
    {
    case (int)CParameterProxy::TYPE::No: {
        nRet = m_uiSocket5->slotAccept(false);
        break;
    }
    case (int)CParameterProxy::TYPE::Socket5: {
        nRet = m_uiSocket5->slotAccept(true);
        if(nRet) return nRet;
        break;
    }
    }

    return 0;
}