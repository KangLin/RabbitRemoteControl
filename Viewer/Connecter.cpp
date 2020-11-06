#include "Connecter.h"

CConnecter::CConnecter(QObject *parent) : QObject(parent)
{}

QString CConnecter::Name()
{
    return Protol();
}

QString CConnecter::Description()
{
    return Name();
}
