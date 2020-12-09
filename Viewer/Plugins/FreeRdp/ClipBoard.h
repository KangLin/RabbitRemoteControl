//! @author: Kang Lin(kl222@126.com)

#ifndef CCLIPBOARD_H
#define CCLIPBOARD_H

#include <QObject>
#include "freerdp/freerdp.h"
#include "freerdp/client/cliprdr.h"
#include "freerdp/client/rdpgfx.h"

class CClipBoard : public QObject
{
    Q_OBJECT
public:
    explicit CClipBoard(QObject *parent = nullptr);
    
    int Init(CliprdrClientContext *cliprdr);
    int UnInit(CliprdrClientContext* cliprdr);
    
    static UINT xf_cliprdr_monitor_ready(CliprdrClientContext* context,
                                         const CLIPRDR_MONITOR_READY* monitorReady);
    static UINT xf_cliprdr_server_capabilities(CliprdrClientContext* context,
                                               const CLIPRDR_CAPABILITIES* capabilities);
    
};

#endif // CCLIPBOARD_H
