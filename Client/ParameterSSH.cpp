#include "ParameterSSH.h"

CParameterSSH::CParameterSSH(CParameterConnecter *parent): CParameterConnecter(parent)
{}

int CParameterSSH::onLoad(QSettings &set)
{
    int nRet = 0;
    nRet = m_Channel.Load(set);
    return nRet;
}

int CParameterSSH::onSave(QSettings &set)
{
    int nRet = 0;
    nRet = m_Channel.Save(set);
    return nRet;
}
