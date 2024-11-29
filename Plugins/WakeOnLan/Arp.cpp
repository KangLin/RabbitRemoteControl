// Author: Kang Lin <kl222@126.com>
#include <QLoggingCategory>
#include <QMutexLocker>

#ifdef HAVE_PCAPPLUSPLUS
#include <Packet.h>
#include <EthLayer.h>
#include <ArpLayer.h>
#include <PcapLiveDeviceList.h>
#include <PcapFilter.h>
#ifdef _MSC_VER
#include <SystemUtils.h>
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif
#endif

#include "RabbitCommonTools.h"

#include "Arp.h"

static Q_LOGGING_CATEGORY(log, "WOL")
CArp::CArp(QObject *parent) : QObject(parent)
{
    qDebug(log) << Q_FUNC_INFO;

    m_Timer.setInterval(1000);
    bool check = connect(&m_Timer, SIGNAL(timeout()),
                         this, SLOT(slotProcess()));
    Q_ASSERT(check);
}

CArp::~CArp()
{
    qDebug(log) << Q_FUNC_INFO;
#ifdef HAVE_PCAPPLUSPLUS
    StopCapture();
#endif
}

int CArp::WakeOnLan(QSharedPointer<CParameterWakeOnLan> para)
{
    int nRet = 0;

    if(!para)
        return -1;

    if(para->GetMac().isEmpty())
    {
        qCritical(log) << "The mac address is empty";
        return -2;
    }
#ifdef HAVE_PCAPPLUSPLUS
    auto it = m_Para.find(para->m_Net.GetHost().toStdString());
    if(it != m_Para.end())
    {
        if((*it)->bWakeOnLan) {
            qDebug(log) << "The wake on lan is existed"
                        << (*it)->para->m_Net.GetHost();
            return 0;
        }
    }

    m_Wol.SetBroadcastAddress(para->GetBroadcastAddress());
    if(para->GetPassword().isEmpty())
        m_Wol.SendMagicPacket(para->GetMac());
    else
        m_Wol.SendSecureMagicPacket(para->GetMac(), para->GetPassword());

#if defined(Q_OS_UNIX)
    if(RabbitCommon::CTools::HasAdministratorPrivilege())
    {
        QSharedPointer<ArpRequest> aq(new ArpRequest(para));
        if(!aq) {
            qCritical(log) << "new ArpRequest fail";
            return -3;
        }
        aq->bWakeOnLan = true;
        nRet = GetMac(para, aq);
    }
#else
    QSharedPointer<ArpRequest> aq(new ArpRequest(para));
    if(!aq) {
        qCritical(log) << "new ArpRequest fail";
        return -3;
    }
    aq->bWakeOnLan = true;
    nRet = GetMac(para, aq);
#endif

#else
    qDebug(log) << "There is not pcapplusplus";
#endif
    return nRet;
}

#ifdef HAVE_PCAPPLUSPLUS
int CArp::StopCapture()
{
    const std::vector<pcpp::PcapLiveDevice*>& devList =
        pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDevicesList();

    qDebug(log) << Q_FUNC_INFO;
    for (const auto& dev : devList)
    {
        if(dev->captureActive())
            dev->stopCapture();
        if(dev->isOpened())
            dev->close();
    }
    return 0;
}

int CArp::SendArpPackage(pcpp::PcapLiveDevice* device,
                         std::string szSourceIp, std::string szTargetIp)
{
    pcpp::MacAddress sourceMac;
    pcpp::IPv4Address sourceIP(szSourceIp);
    pcpp::IPv4Address targetIP(szTargetIp);

    if(!device || szSourceIp.empty() || szTargetIp.empty())
        return -1;
    if(!pcpp::IPv4Address::isValidIPv4Address(szSourceIp))
        sourceIP = device->getIPv4Address();
    sourceMac = device->getMacAddress();

    // create an ARP request from sourceMac and sourceIP and ask for target IP
    pcpp::Packet arpRequest(100);
    pcpp::MacAddress destMac(0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
    pcpp::EthLayer ethLayer(sourceMac, destMac);
    pcpp::ArpLayer arpLayer(pcpp::ARP_REQUEST,
                            sourceMac, destMac, sourceIP, targetIP);
    if (!arpRequest.addLayer(&ethLayer))
    {
        qCritical(log) << "Couldn't build Eth layer for ARP request";
        return -2;
    }
    if (!arpRequest.addLayer(&arpLayer))
    {
        qCritical(log) << "Couldn't build ARP layer for ARP request";
        return -3;
    }
    arpRequest.computeCalculateFields();
    // send the ARP request
    bool bRet = device->sendPacket(&arpRequest);
    if(bRet) return 0;
    return -4;
}

static void cbArpPacketReceived(pcpp::RawPacket* rawPacket,
                                pcpp::PcapLiveDevice*, void* userCookie)
{
    using namespace pcpp;

    // parse the response packet
    Packet packet(rawPacket);

    // verify that it's an ARP packet
    // (although it must be because I set an ARP reply filter on the interface)
    if (!packet.isPacketOfType(ARP))
        return;

    // extract the ARP layer from the packet
    ArpLayer* arpReplyLayer = packet.getLayerOfType<ArpLayer>(true);  // lookup in reverse order
    if (arpReplyLayer == nullptr)
        return;

    // verify it's the right ARP response
    if (arpReplyLayer->getArpHeader()->hardwareType != htons(1) /* Ethernet */
        || arpReplyLayer->getArpHeader()->protocolType != htons(PCPP_ETHERTYPE_IP))
        return;

    // get the data from the main thread
    CArp* pThis = reinterpret_cast<CArp*>(userCookie);
    if(!pThis)
    {
        qDebug(log) << "The use data is nullptr";
        return;
    }

    // verify the ARP response is the response for out request
    // (and not some arbitrary ARP response)
    std::string szTarget = arpReplyLayer->getSenderIpAddr().toString();
    QMutexLocker lock(&pThis->m_Mutex);
    auto it = pThis->m_Para.find(szTarget);
    if(pThis->m_Para.end() == it)
        return;

    auto para = it.value()->para;
    para->SetMac(arpReplyLayer->getSenderMacAddress().toString().c_str());
    para->SetHostState(CParameterWakeOnLan::HostState::Online);
    it.value()->nTimeout = 0;
}

void CArp::ListInterfaces()
{
    const std::vector<pcpp::PcapLiveDevice*>& devList =
        pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDevicesList();

    qDebug(log) << "Network interfaces:";
    for (const auto& dev : devList)
    {
        qDebug(log) << "  -> Name:" << dev->getName().c_str();
        for(auto addr: dev->getIPAddresses())
            qDebug(log) << "    " << addr.toString().c_str();
    }
    return;
}
#endif //#ifdef HAVE_PCAPPLUSPLUS

int CArp::GetMac(QSharedPointer<CParameterWakeOnLan> para
#ifdef HAVE_PCAPPLUSPLUS
                 , QSharedPointer<ArpRequest> ar
#endif
                 )
{
    qDebug(log) << Q_FUNC_INFO << para;
    int nRet = 0;
    if(!para) return -1;
#ifdef HAVE_PCAPPLUSPLUS
    std::string szSourceIp;
    if(!para->GetNetworkInterface().isEmpty())
        szSourceIp = para->GetNetworkInterface().toStdString();
    std::string szTargetIp;
    if(!para->m_Net.GetHost().isEmpty())
        szTargetIp = para->m_Net.GetHost().toStdString();

    pcpp::PcapLiveDevice* device = nullptr;

    try{
        //ListInterfaces();
        if(!pcpp::IPv4Address::isValidIPv4Address(szTargetIp)) {
            qCritical(log) << "Target ip is invalid:" << szTargetIp.c_str();
            return -1;
        }

        device = pcpp::PcapLiveDeviceList::getInstance()
                     .getPcapLiveDeviceByIpOrName(szSourceIp);
        if (device == nullptr) {
            qCritical(log)
            << "Couldn't find interface by provided IP address or name"
            << szSourceIp.c_str();
            return -2;
        }

        if(!device->isOpened())
            if(!device->open()) {
                qCritical(log) << "Open device fail" << szSourceIp.c_str();
                return -3;
            }

        if(!device->captureActive()) {
            // set a filter for the interface to intercept only ARP response packets
            pcpp::ArpFilter arpFilter(pcpp::ARP_REPLY);
            if (!device->setFilter(arpFilter))
            {
                qCritical(log) << "Couldn't set ARP filter for device";
                return -6;
            }
            // start capturing. The capture is done on another thread,
            // hence "arpPacketReceived" is running on that thread
            device->startCapture(cbArpPacketReceived, this);
        }

        if(m_Para.end() == m_Para.find(szTargetIp)) {
            m_Mutex.lock();
            auto ar = QSharedPointer<ArpRequest>(new ArpRequest(para));
            m_Para.insert(szTargetIp, ar);
            m_Mutex.unlock();
        }
        nRet = SendArpPackage(device, szSourceIp, szTargetIp);
        if(!m_Timer.isActive())
            m_Timer.start();

        return nRet;
    } catch(std::exception e) {
        qDebug(log) << "std::exception" << e.what();
    } catch(...) {
        qDebug(log) << "Exception";
    }
#else
    qDebug(log) << "There is not pcapplusplus";
#endif //   #ifdef HAVE_PCAPPLUSPLUS
    return -10;
}

void CArp::slotProcess()
{
    qDebug(log) << Q_FUNC_INFO;
#ifdef HAVE_PCAPPLUSPLUS
    m_Mutex.lock();
    for(auto it = m_Para.begin(); it != m_Para.end();)
    {
        auto a = it.value();
        auto para = a->para;

        // Get mac address fail
        if(a->tmStart.msecsTo(QTime::currentTime()) > a->nTimeout) {
            qDebug(log) << "Get mac address fail" << para->m_Net.GetHost()
                        << a->nRepeat;
            m_Para.erase(it);
            it = m_Para.begin();
            if(a->nTimeout)
                para->SetHostState(CParameterWakeOnLan::HostState::Offline);
            continue;
        }

        // Wake on lan
        if(a->bWakeOnLan && a->nRepeat > 0
            && a->tmRepeat.msecsTo(QTime::currentTime()) > para->GetInterval())
        {
            qDebug(log) << "Repeat wake on lan" << para->m_Net.GetHost()
                        << a->nRepeat;
            m_Wol.SetBroadcastAddress(para->GetBroadcastAddress());
            if(para->GetPassword().isEmpty())
                m_Wol.SendMagicPacket(para->GetMac());
            else
                m_Wol.SendSecureMagicPacket(para->GetMac(), para->GetPassword());
        }

        if(a->nRepeat > 0
            && a->tmRepeat.msecsTo(QTime::currentTime()) > para->GetInterval())
        {
            qDebug(log) << "Repeat get mac address" << para->m_Net.GetHost()
                        << a->nRepeat;
            GetMac(para);
        }

        a->tmRepeat = QTime::currentTime();
        a->nRepeat--;
        it++;
    }
    m_Mutex.unlock();
    if(m_Para.isEmpty()) {
        m_Timer.stop();
        StopCapture();
    }
#endif
}
