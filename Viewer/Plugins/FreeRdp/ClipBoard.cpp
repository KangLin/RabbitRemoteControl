#include "ClipBoard.h"

CClipBoard::CClipBoard(QObject *parent) : QObject(parent)
{}

int CClipBoard::Init(CliprdrClientContext *cliprdr)
{
    cliprdr->custom = this;
    return 0;
}

int CClipBoard::UnInit(CliprdrClientContext *cliprdr)
{
    cliprdr->custom = nullptr;
    return 0;
}
