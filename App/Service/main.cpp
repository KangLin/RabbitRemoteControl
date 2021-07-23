
#include "ServiceManager.h"

int main(int argc, char *argv[])
{
    int nRet = 0;
    CServiceManager s(argc, argv);
    s.exec();
    return nRet;
}
