#include "ParameterUI.h"

CParameterUI::CParameterUI(QWidget *parent)
    : QWidget{parent}
{}

bool CParameterUI::CheckValidity(bool validity)
{
    Q_UNUSED(validity)
    return true;
}

void CParameterUI::slotSetParameter(CParameter *pParameter)
{
    SetParameter(pParameter);
}

void CParameterUI::slotAccept()
{
    Accept();
}
