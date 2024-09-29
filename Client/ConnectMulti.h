#ifndef CONNECTMULTI_H
#define CONNECTMULTI_H

#pragma once

#include "ConnecterConnect.h"
#include "QQueue"

class CConnectMulti : public QObject
{
    Q_OBJECT
public:
    CConnectMulti(QObject* parent = nullptr);
    virtual ~CConnectMulti();

    int Init(CConnecterConnect* pConnecter);
    int Clean();
public Q_SLOTS:
    void slotNext();

private:
    QQueue<CConnect*> m_Connects;
    CConnect* m_pCurrent;
};

#endif // CONNECTMULTI_H
