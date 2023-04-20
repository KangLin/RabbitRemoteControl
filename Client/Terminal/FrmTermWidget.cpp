#include "FrmTermWidget.h"
#include <QDebug>
#include <QMenu>
#include <QApplication>
#include <QDesktopServices>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Terminal)

CFrmTermWidget::CFrmTermWidget(QWidget *parent) : QTermWidget(0, parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    bool check = connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(slotCustomContextMenuCall(const QPoint &)));
    Q_ASSERT(check);

    check = connect(this, SIGNAL(urlActivated(const QUrl&, bool)),
                    this, SLOT(slotActivateUrl(const QUrl&, bool)));
    Q_ASSERT(check);
}

CFrmTermWidget::~CFrmTermWidget()
{
    qDebug(Terminal) << "CFrmTermWidget::~CFrmTermWidget()";
}

void CFrmTermWidget::slotCustomContextMenuCall(const QPoint & pos)
{
    QMenu menu;
    
    menu.addAction(tr("Copy selection to clipboard"), this, SLOT(copyClipboard()));
    menu.addAction(tr("Paste clipboard"), this, SLOT(pasteClipboard()));
    menu.addAction(tr("Paste selection"), this, SLOT(pasteSelection()));
    menu.addSeparator();
    menu.addAction(tr("Zoom in"), this, SLOT(zoomIn()));
    menu.addAction(tr("Zoom out"), this, SLOT(zoomOut()));
    menu.addAction(tr("Zoom reset"), this, SIGNAL(sigZoomReset()));
    menu.addSeparator();
    menu.addAction(tr("Clear"), this, SLOT(clear()));
    
    menu.exec(mapToGlobal(pos));
}

void CFrmTermWidget::slotActivateUrl(const QUrl& url, bool fromContextMenu)
{
    if (QApplication::keyboardModifiers() & Qt::ControlModifier || fromContextMenu) {
        QDesktopServices::openUrl(url);
    }
}
