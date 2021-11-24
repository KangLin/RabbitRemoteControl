#include "Desktop.h"
#include <QApplication>
#include <QScreen>
#include <QDesktopWidget>
#include <QDebug>

CDesktop::CDesktop(QObject *parent) : QObject(parent)
{}

CDesktop::~CDesktop()
{}

CDesktop* CDesktop::Instance()
{
    CDesktop* p = nullptr;
    if(!p)
        p = new CDesktop();
    return p;
}

int CDesktop::Width()
{
    return qApp->desktop()->width();
}

int CDesktop::Height()
{
    return qApp->desktop()->height();
}

QImage CDesktop::GetDesktop(int width, int height, int x, int y)
{
    QDesktopWidget* pDesktop = qApp->desktop();
//    QPixmap pix(pDesktop->size());
//    pDesktop->render(&pix);
    
    if(-1 == width)
        width = Width();
    if(-1 == height)
        height = Height();
    QPixmap pix(width, height);
    pDesktop->render(&pix, QPoint(0, 0), QRegion(x, y, width, height));
    QImage img = pix.toImage();
    return img;
}
