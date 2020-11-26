#include "Connecter.h"

CConnecter::CConnecter(QObject *parent) : QObject(parent),
    m_pView(new CFrmViewer())
{}

CFrmViewer* CConnecter::GetViewer()
{
    return m_pView;
}
