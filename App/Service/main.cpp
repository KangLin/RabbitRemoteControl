// Author: Kang Lin <kl222@126.com>

#include "ServiceManager.h"

int main(int argc, char *argv[])
{
    int nRet = 0;

    CServiceManager s(argc, argv, "RabbitRemoteControlService");
    s.exec();
    return nRet;
}
